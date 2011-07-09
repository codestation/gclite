/*
 *  this file is part of Game Categories Lite
 *  Contain parts of Game Categories Revised/Light
 *
 *  Copyright (C) 2009-2011, Bubbletune
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

#include "categories_lite.h"
#include "logger.h"

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

void gc_utf8_to_unicode(wchar_t *dest, char *src) {
    int i;

    for (i = 0; i == 0 || src[i - 1]; i++) {
        dest[i] = src[i];
    }
}
