/*
	Game Categories Light v 1.3
	Copyright (C) 2011, Bubbletune
	
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

//#define FOLDERS_N 4

//SceUID dir_fd[FOLDERS_N];

extern char category[52];

//extern char *folders[FOLDERS_N];

//extern SceUID gc_sema;

//int fake_game_folder = 0;

/* Global variables */
extern char user_buffer[256];
//SceUID opened_dfd = -1;
//SceUID game_dfd = -1;
//int uncategorized;
//extern u32 text_addr_game;
//int hide_uncategorized;


char mod_path[64];
char orig_path[64];
int multi_cat = 0;
SceUID catdfd = -1;

int is_iso_cat() {
    sce_paf_private_snprintf(user_buffer, 256, "ms0:/ISO/CAT_%s", category);
    SceIoStat st;
    memset(&st, 0, sizeof(SceIoStat));
    if(sceIoGetstat(user_buffer, &st) >= 0 && FIO_S_ISDIR(st.st_mode)) {
        return 1;
    }
    return 0;
}

inline void fix_path(char **path) {
    if(*category && sce_paf_private_strcmp(*path, mod_path) == 0 && is_iso_cat()) {
        *path = orig_path;
    }
}

int is_category_folder(SceIoDirent *dir, char *cat) {
    if(FIO_S_ISDIR(dir->d_stat.st_mode)) {
        if(sce_paf_private_strncmp(dir->d_name, "CAT_", 4) == 0) {
            if(!cat || sce_paf_private_strcmp(dir->d_name + 4, cat) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

SceUID open_iso_cat(SceUID fd) {
    SceIoDirent dir;
    if(fd >= 0) {
        memset(&dir, 0, sizeof(SceIoDirent));
        while(1) {
            int res = sceIoDread(fd, &dir);
            if(res > 0) {
                kprintf("open_iso_cat, check: %s\n", dir.d_name);
                if(is_category_folder(&dir, category)) {
                    // full path
                    sce_paf_private_snprintf(user_buffer, 256, "%s/%s", orig_path, dir.d_name);
                    kprintf("open_iso_cat, opening: %s\n", user_buffer);
                    fd = sceIoDopen(user_buffer);
                    break;
                }
            } else {
                fd = -1;
                break;
            }
        }
    }
    return fd;
}

SceUID sceIoDopenPatched(const char *path) {
    kprintf("sceIoDopenPatched called: %s\n", path);
    if(*category && sce_paf_private_strcmp(path, mod_path) == 0 && is_iso_cat()) {
        multi_cat = 1;
        path = orig_path;
        kprintf("Changed path to: %s\n", path);
    }
    return sceIoDopen(path);
}

//int sceIoDclosePatched(SceUID fd) {
//    return sceIoDclose(fd);
//}

// skips the categories in default mode
int sceIoDreadPatched(SceUID fd, SceIoDirent *dir) {
    int res = -1;
    //kprintf("sceIoDreadPatched called\n");
    while(1) {
        if(catdfd >= 0) {
            kprintf("Reading %s\n", category);
            res = sceIoDread(catdfd, dir);
            if(res <= 0) {
                sceIoDclose(catdfd);
                kprintf("Open next category\n");
                if((catdfd = open_iso_cat(fd)) < 0) {
                    multi_cat = 0;
                    break;
                }
                continue;
            }
            kprintf("Read %s\n", dir->d_name);
            break;
        }
        if(multi_cat) {
            kprintf("Found iso category: %s\n", category);
            if((catdfd = open_iso_cat(fd)) < 0) {
                multi_cat = 0;
                break;
            }
            continue;
        }
        res = sceIoDread(fd, dir);
        // filter out category folders in uncategorized view
        if(category[0] == '\0' && res > 0) {
            kprintf("Checking: %s\n", dir->d_name);
            if(is_category_folder(dir, NULL)) {
                kprintf("Skipping %s\n", dir->d_name);
                continue;
            }
        }
        break;
    }
    return res;
}

//SceUID sceIoOpenPatched(const char *file, int flags, SceMode mode) {
//    return sceIoOpen(file, flags, mode);
//}

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

int sce_paf_private_snprintf_patched(char *a0, int a1, const char *a2, void *a3, void *t0)
{
	sce_paf_private_strcpy((char *)a1, (char *)t0);
	return sce_paf_private_snprintf(a0, 291, a2, a3, t0);
}

char *(*GetBasePath)(int arg);

char *ReturnBasePatched(char *base) {
    if(*category && base && sce_paf_private_strcmp(base + 4, "/PSP/GAME") == 0) {
        sce_paf_private_strcpy(orig_path, base);
        sce_paf_private_strcpy(mod_path, base);
        sce_paf_private_strcpy(mod_path + 13, "/CAT_");
        sce_paf_private_strcpy(mod_path + 18, category);
        return mod_path;
    }
    return base;
}

void PatchIoFileMgrForGamePlugin(u32 text_addr)
{
	//MAKE_STUB(text_addr+PATCHES->io_dopen_stub, sceIoDopenPatched); //nil
	MAKE_STUB(text_addr+PATCHES->io_dread_stub, sceIoDreadPatched); //nil
	//MAKE_STUB(text_addr+PATCHES->io_dclose_stub, sceIoDclosePatched); //nil
	//MAKE_STUB(text_addr+PATCHES->io_open_stub, sceIoOpenPatched); //nil
	MAKE_STUB(text_addr+PATCHES->io_getstat_stub, sceIoGetstatPatched); //nil
	MAKE_STUB(text_addr+PATCHES->io_chstat_stub, sceIoChstatPatched); //nil
	MAKE_STUB(text_addr+PATCHES->io_remove_stub, sceIoRemovePatched); //nil
	MAKE_STUB(text_addr+PATCHES->io_rmdir_stub, sceIoRmdirPatched); //nil

    MAKE_JUMP(text_addr + 0x21310, ReturnBasePatched);
    _sw(0x00602021, text_addr + 0x21314); // move $a0, $v1

    //GetBasePath = (void *)(U_EXTRACT_CALL(text_addr+0x1CDE8));
    //MAKE_CALL(text_addr+0x1CDE8, GetBasePathPatched);
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
