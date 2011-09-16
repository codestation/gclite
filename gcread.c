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
#include "vshitem.h"
#include "config.h"
#include "filter.h"
#include "language.h"
#include "logger.h"

#define GAME_FOLDER "/PSP/GAME"

//extern variables
extern Category *folder_list[2];

// global vars
char category[52];

// unit vars
static SceUID game_dfd = -1;
static SceUID fakefd = -1;
static SceUID realfd = -1;
static SceUID openfd = -1;
static int uncategorized;
static char user_buffer[256];
static char mod_base[128];
static char opened_path[128];

inline void trim(char *str) {
    int i = sce_paf_private_strlen(str);
    while(str[i-1] == ' ') {
        --i;
    }
    if(str[i] == ' ') {
        str[i] = '\0';
    }
}

int is_category_folder(SceIoDirent *dir) {
    kprintf("checking %s\n", dir->d_name);
    if(FIO_S_ISDIR(dir->d_stat.st_mode)) {
        if(!*category) {
            if(config.mode == MODE_FOLDER) {
                kprintf("base: %s\n", opened_path);
                if(!config.prefix && (*opened_path && !is_game_folder(opened_path, dir->d_name)) && !FindCategory(folder_list, dir->d_name, global_pos)) {
                    return 1;
                }
            } else {
                if(!config.prefix && FindCategory(cat_list, dir->d_name, global_pos)) {
                    return 1;
                }
            }
            if(config.prefix && sce_paf_private_strncmp(dir->d_name, "CAT_", 4) == 0) {
                return 1;
            }
        }
        if(!config.prefix && sce_paf_private_strcmp(dir->d_name, category) == 0) {
            return 1;
        }
        if(config.prefix && sce_paf_private_strcmp(dir->d_name + 4, category) == 0) {
            return 1;
        }
    }
    return 0;
}


SceUID sceIoDopenPatched(const char *path) {
    SceUID fd = sceIoDopen(path);

    // only make a backup of the opened path if the game folder is opened in uncategorized mode
    if (!*category && sce_paf_private_strcmp(path + 4, GAME_FOLDER) == 0) {
        sce_paf_private_strcpy(opened_path, path);
    } else {
        *opened_path = '\0';
    }

    if(config.mode == MODE_FOLDER && sce_paf_private_strcmp(path + 4, GAME_FOLDER) == 0) {
        sce_paf_private_strcpy(opened_path, path);
        ClearCategories(folder_list, global_pos);
        uncategorized = 0;
        game_dfd = fd;
    }

    kprintf("opened dir, path: [%s], fd: %08X\n", path, fd);
    // we are receiving a kernel mode file descriptor
    if(fd > 0xFFFF) {
        realfd = fd;
        sce_paf_private_strncpy(user_buffer, path, 13);
        user_buffer[13]= '\0';
        // lets return a dummy fd instead
        fakefd = sceIoDopen(user_buffer);
        kprintf("Opening fake dir: [%s], fd: %08X\n", user_buffer, fakefd);
        fd = fakefd;
    }
    return fd;
}

int sceIoDreadPatchedFolder(SceUID fd, SceIoDirent *dir) {
    int res;

    if (fd == game_dfd) {
        while (1) {
            if (openfd >= 0) {
                res = sceIoDread(openfd, dir);
                if (res > 0) {
                    if (dir->d_name[0] != '.' && !check_filter(dir->d_name)) {
                        sce_paf_private_strcpy(dir->d_name + 128, dir->d_name);
                        sce_paf_private_snprintf(dir->d_name, 128, "%s/%s", user_buffer + 14, dir->d_name + 128);
                        if (dir->d_private) {
                            sce_paf_private_strcpy((char *)dir->d_private + 13, dir->d_name);
                        }
                        kprintf("B) exit, dir: [%s]\n", dir->d_name);
                        return res;
                    } else {
                        kprintf("C) ignoring [%s]\n", dir->d_name);
                        continue;
                    }
                } else {
                    sceIoDclose(openfd);
                    openfd = -1;
                }
            }

            res = sceIoDread(fd, dir);

            if (res > 0) {
                kprintf("checking %s\n", dir->d_name);
                if (dir->d_name[0] != '.') {
                    if(is_category_folder(dir) && has_directories(opened_path, dir->d_name)) {
                        u64 mtime;

                        kprintf("category match: %s\n", dir->d_name);
                        sceRtcGetTick((pspTime *) &dir->d_stat.st_mtime, &mtime);
                        kprintf("Adding %s\n", dir->d_name);
                        AddCategory(folder_list, dir->d_name, mtime, global_pos);
                        sce_paf_private_snprintf(user_buffer, 128, "%s/%s", opened_path, dir->d_name);
                        openfd = sceIoDopen(user_buffer);
                        continue;
                    } else {
                        if (!global_pos && (config.uncategorized & ONLY_MS)) {
                            uncategorized = 1;
                        } else if (global_pos && (config.uncategorized & ONLY_IE)) {
                            uncategorized = 1;
                        } else {
                            kprintf("A) ignoring [%s]\n", dir->d_name);
                            continue; // ignore this Dread
                        }
                        if(dir->d_name[0] == '.' || check_filter(dir->d_name) || (*opened_path && !is_game_folder(opened_path, dir->d_name))) { // ignore non game folders
                            kprintf("B) ignoring [%s]\n", dir->d_name);
                            continue;
                        }
                    }
                }
            }
            if(res > 0) {
                kprintf("A) exit, dir: [%s]\n", dir->d_name);
            } else {
                kprintf("exit, end of directory\n");
            }
            return res;
        }
    }

    return sceIoDread(fd, dir);
}

int sceIoDreadPatched(SceUID fd, SceIoDirent *dir) {
    SceUID ret;

    //if our fake fd is being used then replace it with the kernel one
    if(fd == fakefd) {
        kprintf("Replacing fakefd: %08X with realfd: %08X\n", fakefd, realfd);
        fd = realfd;
    }

    while((ret = sceIoDread(fd, dir)) > 0) {
        kprintf("read dir: [%s]\n", dir->d_name);
        if(!check_filter(dir->d_name) && (*category || !*opened_path || dir->d_name[0] == '.' || is_game_folder(opened_path, dir->d_name))) {
            break;
        }
    }
    if(ret <= 0) {
        kprintf("end of directory reached\n");
    }
    return ret;
}

int gcGetStatIso(SceIoStat *stat) {
    if(config.prefix) {
        sce_paf_private_strcpy(user_buffer, "xxx:/ISO/CAT_");
        sce_paf_private_strcpy(user_buffer + 13, category);
    } else {
        sce_paf_private_strcpy(user_buffer, "xxx:/ISO/");
        sce_paf_private_strcpy(user_buffer + 9, category);
    }
    SET_DEVICENAME(user_buffer, global_pos);

    // trim away the ME marker (5 spaces at the end of the category)
    trim(user_buffer);

    sce_paf_private_memset(stat, 0, sizeof(SceIoStat));
    kprintf("opening [%s]\n", user_buffer);
    return sceIoGetstat(user_buffer, stat);
}

int sceIoGetstatPatched(char *file, SceIoStat *stat) {
    int ret;

    kprintf("checking [%s]\n", file);
    ret = sceIoGetstat(file, stat);
    if(ret < 0 && *category) {
        // lets verify if it was trying to open a ISO category
        sce_paf_private_strcpy(user_buffer, GAME_FOLDER);
        user_buffer[9] = '/';
        sce_paf_private_strcpy(user_buffer + 10, category);
        if(sce_paf_private_strcmp(user_buffer, file + 4) == 0) {
            kprintf("tried to open a iso category, retry\n");
            // check if the category exists in /ISO
            ret = gcGetStatIso(stat);
            kprintf("gcGetStatIso result: %08X\n", ret);
        }
    }
    return ret;
}

char *ReturnBasePathPatched(char *base) {
    kprintf("orig base: [%s]\n", base);
    // only do the patch if a category is being accessed
    if(*category && base && sce_paf_private_strcmp(base + 4, GAME_FOLDER) == 0) {
        sce_paf_private_strcpy(mod_base, base);
        //append the category dir if is available and the original path is /PSP/GAME
        if(config.prefix) {
            sce_paf_private_strcpy(mod_base + 13, "/CAT_");
            sce_paf_private_strcpy(mod_base + 18, category);
        } else {
            mod_base[13] = '/';
            sce_paf_private_strcpy(mod_base + 14, category);
        }
        // force the device name
        SET_DEVICENAME(mod_base, global_pos);
        kprintf("modified base: [%s]\n", mod_base);
        base = mod_base;
    }
    return base;
}

int sceIoDclosePatched(SceUID fd) {
    kprintf("closing dir, fd: %08X\n", fd);
    if(config.mode == MODE_FOLDER && fd == game_dfd) {
        // add the uncategorized content in folder mode
        if(uncategorized) {
            AddCategory(folder_list, lang_container.msg_uncategorized, 1, 0);
        }
        game_dfd = -1;
    }
    // close the kernel descriptor along the fake one
    if(fd == fakefd) {
        kprintf("closing realfd: %08X\n", realfd);
        sceIoDclose(realfd);
        fakefd = -1;
    }
    return sceIoDclose(fd);
}

int sce_paf_private_snprintf_patched(char *a0, int a1, const char *a2, void *a3, void *t0) {
    sce_paf_private_strcpy((char *)a1, (char *)t0);
    return sce_paf_private_snprintf(a0, 291, a2, a3, t0);
}


void PatchGamePluginForGCread(u32 text_addr) {
    // hook some sceIo funcs
    MAKE_STUB(text_addr + patches.io_dread_stub[patch_index], config.mode == MODE_FOLDER ? sceIoDreadPatchedFolder : sceIoDreadPatched);
    MAKE_STUB(text_addr + patches.io_dopen_stub[patch_index], sceIoDopenPatched);
    MAKE_STUB(text_addr + patches.io_dclose_stub[patch_index], sceIoDclosePatched);
    MAKE_STUB(text_addr + patches.io_getstat_stub[patch_index], sceIoGetstatPatched);

    // hook the base path creation
    MAKE_JUMP(text_addr + patches.base_path[patch_index], ReturnBasePathPatched);
    _sw(0x00602021, text_addr + patches.base_path_arg[patch_index]); // move $a0, $v1

    /* SCE renames folders before removal, but it doesn't handle
        categories in doing so. It will try to move things out of
        the category with the rename function... just get rid of
        renaming to fix this lame bug. */
    // #1
    MAKE_CALL(text_addr+patches.snprintf_call_arg_1[patch_index][0], sce_paf_private_snprintf_patched);
    _sw(0x02402821, text_addr+patches.snprintf_call_arg_1[patch_index][1]); // li $a1, 291 -> move $a1, $s2

    // #2
    MAKE_CALL(text_addr+patches.snprintf_call_arg_2[patch_index][0], sce_paf_private_snprintf_patched);
    _sw(0x02002821, text_addr+patches.snprintf_call_arg_2[patch_index][1]); // li $a1, 291 -> move $a1, $s0
}
