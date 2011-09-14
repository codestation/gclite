/*
 *  this file is part of Game Categories Lite
 *  Contain parts of Game Categories Revised/Light
 *  Contain parts of 6.39 TN-A, XmbControl
 *
 *  Copyright (C) 2009-2011, Bubbletune
 *  Copyright (C) 2011, Total_Noob
 *  Copyright (C) 2011, Codestation
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <psprtc.h>
#include <pspreg.h>
#include "categories_lite.h"
#include "logger.h"

#define MASKBITS 0x3F
#define MASK2BYTES 0xC0
#define MASK3BYTES 0xE0

void *redir2stub(u32 address, void *stub, void *redir) {
    _sw(_lw(address), (u32)stub);
    _sw(_lw(address + 4), (u32)stub + 4);
    MAKE_JUMP((u32)stub + 8, address + 8);
    MAKE_STUB(address, redir);
    return stub;
}

void *redir_call(u32 address, void *func) {
    void *f = (void *)U_EXTRACT_CALL(address);
    MAKE_CALL(address, func);
    return f;
}

int get_location(int action_arg) {
    if(action_arg == PSPGO_CONTEXT_SENTINEL || action_arg == PSPGO_CONTEXT_SENTINEL+1) {
        return INTERNAL_STORAGE;
    } else if(action_arg == PSPMS_CONTEXT_SENTINEL || action_arg == PSPMS_CONTEXT_SENTINEL+1) {
        return MEMORY_STICK;
    }
    if(action_arg >= 100) {
        if(action_arg >= 1000) {
            return INTERNAL_STORAGE;
        } else {
            return MEMORY_STICK;
        }
    }
    return INVALID;
}

void fix_text_padding(wchar_t *fake, wchar_t *real, wchar_t first, wchar_t last) {
    int i, x, len, found;

    for (len = 0; fake[len]; len++)
        ;

    for (found = 0, i = 0; real[i]; i++) {
        if (real[i] == first) {
            found = 1;
            break;
        }
    }

    if (!found) {
        return;
    }

    sce_paf_private_memmove(&fake[i], fake, ((len + 1) * 2));
    sce_paf_private_memcpy(fake, real, (i * 2));
    len += i;

    for (found = 0, i = 0, x = 0; real[i]; i++) {
        if (!found) {
            if (real[i] == last) {
                found = 1;
            }
            x++;
        }

        if (found) {
            found++;
        }
    }

    if (!found) {
        return;
    }

    sce_paf_private_memcpy(&fake[len], &real[x], (found * 2));
}

int gc_utf8_to_unicode(wchar_t *dest, const char *src) {
    int i, x;
    unsigned char *usrc = (unsigned char *) src;

    for (i = 0, x = 0; usrc[i];) {
        wchar_t ch;

        if (ISSET(usrc[i], MASK3BYTES)) {
            ch = ((usrc[i] & 0x0F) << 12) | ((usrc[i + 1] & MASKBITS) << 6) | (usrc[i + 2] & MASKBITS);
            i += 3;
        }

        else if (ISSET(usrc[i], MASK2BYTES)) {
            ch = ((usrc[i] & 0x1F) << 6) | (usrc[i + 1] & MASKBITS);
            i += 2;
        } else {
            ch = usrc[i];
            i += 1;
        }

        dest[x++] = ch;
    }

    dest[x++] = '\0';

    return x;
}

int get_registry_value(const char *dir, const char *name) {
    int res = -1;
    struct RegParam reg;
    REGHANDLE h;

    sce_paf_private_memset(&reg, 0, sizeof(reg));
    reg.regtype = 1;
    reg.namelen = sce_paf_private_strlen("/system");
    reg.unk2 = 1;
    reg.unk3 = 1;
    sce_paf_private_strcpy(reg.name, "/system");

    if (sceRegOpenRegistry(&reg, 2, &h) >= 0) {
        REGHANDLE hd;

        if (sceRegOpenCategory(h, dir, 2, &hd) >= 0) {
            REGHANDLE hk;
            unsigned int type, size;

            if (sceRegGetKeyInfo(hd, name, &hk, &type, &size) >= 0) {
                if (sceRegGetKeyValue(hd, hk, &res, 4) < 0) {
                    res = -1;
                }
            }

            sceRegCloseCategory(hd);
        }

        sceRegCloseRegistry(h);
    }
    return res;
}

// trim and GetLine are taken from this thread:
// http://forums.mformature.net/showpost.php?p=57856&postcount=24
// information about language codes reversed from recovery.prx
// 'char' -> 'unsigned char' to avoid issues with utf-8

void trim(char *str) {
    int len = sce_paf_private_strlen(str);
    int i;

    for (i = len - 1; i >= 0; i--) {
        if (str[i] == 0x20 || str[i] == '\t') {
            str[i] = 0;
        } else {
            break;
        }
    }
}

int GetLine(char *buf, int size, char *str) {
    unsigned char ch = 0;
    int n = 0;
    int i = 0;
    unsigned char *s = (unsigned char *) str;

    while (1) {
        if (i >= size) {
            break;
        }

        ch = ((unsigned char *) buf)[i];

        if (ch < 0x20 && ch != '\t') {
            if (n != 0) {
                i++;
                break;
            }
        } else {
            *str++ = ch;
            n++;
        }

        i++;
    }

    trim((char *) s);

    return i;
}

