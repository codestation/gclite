/*
 *  this file is part of Game Categories Lite
 *
 *  Copyright (C) 2011  Codestation
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
#include <psprtc.h>
#include <string.h>
#include "categories_lite.h"
#include "psppaf.h"
#include "gcpatches.h"
#include "pspdefs.h"
#include "config.h"
#include "logger.h"

char user_buffer[256];
char category[52];

char mod_path[70];
char orig_path[70];
int type = -1;

inline void trim(char *str) {
    int i = sce_paf_private_strlen(str);
    while(str[i-1] == ' ') {
        --i;
    }
    if(str[i] == ' ') {
        str[i] = '\0';
    }
}

int is_iso_cat(const char *path) {
    SceIoStat st;

    if(config.prefix) {
        sce_paf_private_strcpy(user_buffer, "xxx:/ISO/CAT_");
        sce_paf_private_strcpy(user_buffer + 13, category);
    } else {
        sce_paf_private_strcpy(user_buffer, "xxx:/ISO/");
        sce_paf_private_strcpy(user_buffer + 9, category);
    }
    SET_DEVICENAME(user_buffer, type);

    // workaround for ME bug or "feature"
    trim(user_buffer);

    sce_paf_private_memset(&st, 0, sizeof(SceIoStat));
    return sceIoGetstat(user_buffer, &st) >= 0 && FIO_S_ISDIR(st.st_mode) ? 1 : 0;
}

inline void fix_path(char **path) {
    if(*category && sce_paf_private_strcmp(*path, mod_path) == 0 && is_iso_cat(*path)) {
        *path = orig_path;
    }
}

int is_category_folder(SceIoDirent *dir, char *cat) {
    if(FIO_S_ISDIR(dir->d_stat.st_mode)) {
        if(!cat) {
            if(!config.prefix && FindCategory(dir->d_name, type)) {
                return 1;
            }
            if(config.prefix && sce_paf_private_strncmp(dir->d_name, "CAT_", 4) == 0) {
                return 1;
            }
        }
        if(!config.prefix && sce_paf_private_strcmp(dir->d_name, cat) == 0) {
            return 1;
        }
        if(config.prefix && sce_paf_private_strcmp(dir->d_name + 4, cat) == 0) {
            return 1;
        }
    }
    return 0;
}

int sceIoDreadPatched(SceUID fd, SceIoDirent *dir) {
    int res = -1;
    while(1) {
        res = sceIoDread(fd, dir);
        // filter out category folders in uncategorized view
        if(category[0] == '\0' && res > 0) {
            if(dir->d_name[0] == '.' || is_category_folder(dir, NULL) ||
                // skip the VIDEO folder
                sce_paf_private_strcmp(dir->d_name, "VIDEO") == 0) {
                continue;
            }
        }
        break;
    }
    return res;
}

int sceIoGetstatPatched(char *file, SceIoStat *stat) {
    fix_path(&file);
    return sceIoGetstat(file, stat);
}

int sceIoChstatPatched(char *file, SceIoStat *stat, int bits) {
    fix_path(&file);
    return sceIoChstat(file, stat, bits);
}

int sceIoRemovePatched(char *file) {
    fix_path(&file);
    return sceIoRemove(file);
}

int sceIoRmdirPatched(char *path) {
    fix_path(&path);
    return sceIoRmdir(path);
}

char *ReturnBasePathPatched(char *base) {
    if(*category && base && sce_paf_private_strcmp(base + 4, "/PSP/GAME") == 0) {
        sce_paf_private_strcpy(orig_path, base);
        sce_paf_private_strcpy(mod_path, base);
        if(config.prefix) {
            sce_paf_private_strcpy(mod_path + 13, "/CAT_");
            sce_paf_private_strcpy(mod_path + 18, category);
        } else {
            mod_path[13] = '/';
            sce_paf_private_strcpy(mod_path + 14, category);
        }
        // force the device name
        SET_DEVICENAME(orig_path, type);
        SET_DEVICENAME(mod_path, type);
        return mod_path;
    }
    return base;
}

int sce_paf_private_snprintf_patched(char *a0, int a1, const char *a2, void *a3, void *t0) {
    sce_paf_private_strcpy((char *)a1, (char *)t0);
    return sce_paf_private_snprintf(a0, 291, a2, a3, t0);
}


void PatchGamePluginForGCread(u32 text_addr) {
    MAKE_STUB(text_addr+PATCHES->io_dread_stub, sceIoDreadPatched);
    MAKE_STUB(text_addr+PATCHES->io_getstat_stub, sceIoGetstatPatched);
    MAKE_STUB(text_addr+PATCHES->io_chstat_stub, sceIoChstatPatched);
    MAKE_STUB(text_addr+PATCHES->io_remove_stub, sceIoRemovePatched);
    MAKE_STUB(text_addr+PATCHES->io_rmdir_stub, sceIoRmdirPatched);

    //MAKE_STUB(text_addr+PATCHES->io_dopen_stub, sceIoDopenPatched);
    //MAKE_STUB(text_addr+PATCHES->io_dclose_stub, sceIoDclosePatched);
    //MAKE_STUB(text_addr+PATCHES->io_open_stub, sceIoOpenPatched);

    MAKE_JUMP(text_addr + PATCHES->base_path, ReturnBasePathPatched);
    _sw(0x00602021, text_addr + PATCHES->base_path_arg); // move $a0, $v1

    /* SCE renames folders before removal, but it doesn't handle
        categories in doing so. It will try to move things out of
        the category with the rename function... just get rid of
        renaming to fix this lame bug. */
    // #1
    MAKE_CALL(text_addr+PATCHES->snprintf_call_arg_1[0], sce_paf_private_snprintf_patched);
    _sw(0x02402821, text_addr+PATCHES->snprintf_call_arg_1[1]); // li $a1, 291 -> move $a1, $s2

    // #2
    MAKE_CALL(text_addr+PATCHES->snprintf_call_arg_2[0], sce_paf_private_snprintf_patched);
    _sw(0x02002821, text_addr+PATCHES->snprintf_call_arg_2[1]); // li $a1, 291 -> move $a1, $s0
}
