/*
	Game Categories Lite 1.0
	Copyright (C) 2011, codestation
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <pspsdk.h>
#include <pspkernel.h>
#include <psprtc.h>
#include "psppaf.h"
#include <string.h>
#include "game_categories_light.h"
#include "pspdefs.h"
#include "logger.h"

char user_buffer[256];
char category[52];

char mod_path[70];
char orig_path[70];
int type = -1;

inline void trim(char *str) {
    int i = sce_paf_private_strlen(user_buffer) - 1;
    while(user_buffer[i] == ' ')
        --i;
    ++i;
    if(user_buffer[i] == ' ')
        user_buffer[i] = '\0';
}

int is_iso_cat(const char *path) {
    SceIoStat st;

    sce_paf_private_strcpy(user_buffer, "xxx:/ISO/CAT_");
    sce_paf_private_strcpy(user_buffer + 13, category);
    //sce_paf_private_strcpy(user_buffer, "xxx:/ISO/");
    //sce_paf_private_strcpy(user_buffer + 9, category);
    SET_DEVICENAME(user_buffer, type);

    // workaround for ME bug or "feature"
    trim(user_buffer);

    memset(&st, 0, sizeof(SceIoStat));
    //kprintf("%s: checking if %s is a ISO category\n", __func__, user_buffer);
    if(sceIoGetstat(user_buffer, &st) >= 0 && FIO_S_ISDIR(st.st_mode)) {
        //kprintf("> %s: true\n", __func__);
        return 1;
    }
    //kprintf("> %s: false\n", __func__);
    return 0;
}

inline void fix_path(char **path) {
    if(*category && sce_paf_private_strcmp(*path, mod_path) == 0 && is_iso_cat(*path)) {
        *path = orig_path;
    }
}

int is_category_folder(SceIoDirent *dir, char *cat) {
    if(FIO_S_ISDIR(dir->d_stat.st_mode)) {
        if(sce_paf_private_strncmp(dir->d_name, "CAT_", 4) == 0) {
            if(!cat || sce_paf_private_strcmp(dir->d_name + 4, cat) == 0) {
        //if(!cat || sce_paf_private_strcmp(dir->d_name, cat) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

int sceIoDreadPatched(SceUID fd, SceIoDirent *dir) {
    int res = -1;
    //kprintf("%s: start\n", __func__);
    while(1) {
        res = sceIoDread(fd, dir);
        // filter out category folders in uncategorized view
        if(category[0] == '\0' && res > 0) {
            //kprintf(">> %s: checking: %s\n", __func__, dir->d_name);
            if(dir->d_name[0] == '.' || is_category_folder(dir, NULL) ||
                    sce_paf_private_strcmp(dir->d_name, "VIDEO") == 0) { // skip the VIDEO folder too
                //kprintf(">> %s: skipping %s\n", __func__, dir->d_name);
                continue;
            }
        }
//        if(res > 0) {
//            kprintf(">> %s: read %s\n", __func__, dir->d_name);
//        }
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
    //kprintf("%s: base: %s\n", __func__, base);
    if(*category && base && sce_paf_private_strcmp(base + 4, "/PSP/GAME") == 0) {
        sce_paf_private_strcpy(orig_path, base);
        sce_paf_private_strcpy(mod_path, base);
        sce_paf_private_strcpy(mod_path + 13, "/CAT_");
        sce_paf_private_strcpy(mod_path + 18, category);
        //mod_path[13] = '/';
        //sce_paf_private_strcpy(mod_path + 14, category);
        // force the device name
        SET_DEVICENAME(orig_path, type);
        SET_DEVICENAME(mod_path, type);
        //kprintf("%s: changing %s to %s\n", __func__, base, mod_path);
        return mod_path;
    }
    return base;
}
