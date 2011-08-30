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
#include "language.h"
#include "logger.h"

char user_buffer[256];
char category[52];

char mod_path[70];
char orig_path[70];

// ME variables
int multi_cat = 0;
SceUID catdfd = -1;
extern int me_fw;

SceUID game_dfd = -1;
SceUID opened_dfd = -1;
int uncategorized;
extern Category *folder_list[2];

inline void trim(char *str) {
    int i = sce_paf_private_strlen(str);
    while(str[i-1] == ' ') {
        --i;
    }
    if(str[i] == ' ') {
        str[i] = '\0';
    }
}

int is_iso_cat(const char *cat) {
    SceIoStat st;

    if(config.prefix) {
        sce_paf_private_strcpy(user_buffer, "xxx:/ISO/CAT_");
        sce_paf_private_strcpy(user_buffer + 13, cat);
    } else {
        sce_paf_private_strcpy(user_buffer, "xxx:/ISO/");
        sce_paf_private_strcpy(user_buffer + 9, cat);
    }
    SET_DEVICENAME(user_buffer, global_pos);

    // workaround for ME bug or "feature"
    trim(user_buffer);

    sce_paf_private_memset(&st, 0, sizeof(SceIoStat));
    return sceIoGetstat(user_buffer, &st) >= 0 && FIO_S_ISDIR(st.st_mode) ? 1 : 0;
}

inline void fix_path(char **path) {
    if(*category && sce_paf_private_strcmp(*path, mod_path) == 0 && is_iso_cat(category)) {
        *path = orig_path;
    }
}

int is_category_folder(SceIoDirent *dir, const char *cat) {
    kprintf("checking %s\n", dir->d_name);
    if(FIO_S_ISDIR(dir->d_stat.st_mode)) {
        if(!cat) {
            if(!config.prefix && FindCategory(cat_list, dir->d_name, global_pos)) {
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

SceUID open_iso_cat(SceUID fd, SceIoDirent *dir) {
    if(fd >= 0) {
        while(1) {
            int res = sceIoDread(fd, dir);
            if(res > 0) {
                kprintf("checking %s\n", dir->d_name);
                if(is_category_folder(dir, category)) {
                    // full path
                    sce_paf_private_snprintf(user_buffer, 256, "%s/%s", orig_path, dir->d_name);
                    kprintf("opening iso cat: %s\n", user_buffer);
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
    if(*category && sce_paf_private_strcmp(path, mod_path) == 0 && is_iso_cat(category)) {
        multi_cat = 1;
        path = orig_path;
        kprintf("changed path to: %s\n", path);
    }
    sce_paf_private_strcpy(user_buffer, path);
    kprintf("path: %s\n", path);
    if(*category) {
        kprintf("category: %s\n", category);
    }
    if(config.mode == MODE_FOLDER) {
        kprintf("Folder mode active\n");
        ClearCategories(folder_list, global_pos);
        game_dfd = sceIoDopen(path);
        return game_dfd;
    }
    return sceIoDopen(path);
}

int sceIoDreadPatchedF(SceUID fd, SceIoDirent *dir) {
    if (fd == game_dfd) {
        while (1) {
            if (opened_dfd >= 0) {
                int res = sceIoDread(opened_dfd, dir);
                if (res > 0) {
                    if (dir->d_name[0] != '.') {
                        sce_paf_private_strcpy(dir->d_name + 128, dir->d_name);
                        sce_paf_private_snprintf(dir->d_name, 128, "%s/%s", user_buffer + 14, dir->d_name + 128);
                        if (dir->d_private) {
                            sce_paf_private_strcpy((char *)dir->d_private + 13, dir->d_name);
                        }
                        return res;
                    } else {
                        continue;
                    }
                } else {
                    sceIoDclose(opened_dfd);
                    opened_dfd = -1;
                }
            }

            int res = sceIoDread(fd, dir);

            if (res > 0) {
                kprintf("checking %s\n", dir->d_name);
                if (dir->d_name[0] != '.') {
                    if (FIO_S_ISDIR(dir->d_stat.st_mode)) {
                        SceIoStat stat;
                        sce_paf_private_memset(&stat, 0, sizeof(SceIoStat));
                        sce_paf_private_snprintf(user_buffer + 13, 128,"/%s/EBOOT.PBP", dir->d_name);

                        if (sceIoGetstat(user_buffer, &stat) < 0) {
                            sce_paf_private_snprintf(user_buffer + 13, 128, "/%s/PARAM.PBP", dir->d_name);
                            if (sceIoGetstat(user_buffer, &stat) < 0) {
                                u64 mtime;
                                sceRtcGetTick((pspTime *) &dir->d_stat.st_mtime, &mtime);
                                kprintf("Adding %s\n", dir->d_name);
                                AddCategory(folder_list, dir->d_name, mtime, global_pos);
                                sce_paf_private_snprintf(user_buffer + 13, 128, "/%s", dir->d_name);
                                opened_dfd = sceIoDopen(user_buffer);
                                continue;
                            } else {
                                if (!config.uncategorized) {
                                    continue; // ignore this Dread
                                }
                                uncategorized = 1;
                            }
                        } else {
                            if (!config.uncategorized) {
                                continue; // ignore this Dread
                            }
                            uncategorized = 1;
                        }
                    }
                }
            }
            return res;
        }
    }

    return sceIoDread(fd, dir);
}

int sceIoDreadPatchedME(SceUID fd, SceIoDirent *dir) {
    int res = -1;

    kprintf("start\n");
    while(1) {
        if(catdfd >= 0) {
            kprintf("reading %s\n", category);
            res = sceIoDread(catdfd, dir);
            if(res <= 0) {
                sceIoDclose(catdfd);
                kprintf("open next category\n");
                if((catdfd = open_iso_cat(fd, dir)) < 0) {
                    kprintf("end (no more categories)\n");
                    multi_cat = 0;
                    break;
                }
                continue;
            }
            kprintf("read %s\n", dir->d_name);
            kprintf("end (normal)\n");
            break;
        }
        if(multi_cat) {
            kprintf("found ISO category: %s\n", category);
            if((catdfd = open_iso_cat(fd, dir)) < 0) {
                kprintf("end (no ISO category)\n");
                multi_cat = 0;
                break;
            }
            continue;
        }
        res = sceIoDread(fd, dir);
        // filter out category folders in uncategorized view
        if(category[0] == '\0' && res > 0) {
            kprintf("checking: %s\n", dir->d_name);
            if(dir->d_name[0] == '.' || is_category_folder(dir, NULL) ||
                    sce_paf_private_strcmp(dir->d_name, "VIDEO") == 0) { // skip the VIDEO folder too
                kprintf("skipping %s\n", dir->d_name);
                continue;
            }
        }
        if(res > 0) {
            kprintf("read %s\n", dir->d_name);
        }
        break;
    }
    return res;
}

int sceIoDreadPatched(SceUID fd, SceIoDirent *dir) {
    int res = -1;
    kprintf("called\n");
    while(1) {
        res = sceIoDread(fd, dir);
        // filter out category folders in uncategorized view
        if(category[0] == '\0' && res > 0) {
            kprintf("read %s\n", dir->d_name);
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
    kprintf("checking %s\n", file);
    fix_path(&file);
    kprintf("modcheck %s\n", file);
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
    //kprintf("name: %s\n", base);
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
        SET_DEVICENAME(orig_path, global_pos);
        SET_DEVICENAME(mod_path, global_pos);
        kprintf("modified path: %s\n", mod_path);
        return mod_path;
    }
    return base;
}

int sceIoDclosePatched(SceUID fd) {
    if(fd == game_dfd) {
        if(uncategorized) {
            AddCategory(folder_list, lang_container.msg_uncategorized, 1, 0);
        }
        game_dfd = -1;
    }
    return sceIoDclose(fd);
}

int sce_paf_private_snprintf_patched(char *a0, int a1, const char *a2, void *a3, void *t0) {
    sce_paf_private_strcpy((char *)a1, (char *)t0);
    return sce_paf_private_snprintf(a0, 291, a2, a3, t0);
}


void PatchGamePluginForGCread(u32 text_addr) {
    if(config.mode == MODE_FOLDER) {
        MAKE_STUB(text_addr+patches.io_dread_stub[patch_index], sceIoDreadPatchedF);
    } else {
        if(me_fw) {
            MAKE_STUB(text_addr+patches.io_dread_stub[patch_index], sceIoDreadPatchedME);
        } else {
            MAKE_STUB(text_addr+patches.io_dread_stub[patch_index], sceIoDreadPatched);
        }
    }
    MAKE_STUB(text_addr+patches.io_getstat_stub[patch_index], sceIoGetstatPatched);
    MAKE_STUB(text_addr+patches.io_chstat_stub[patch_index], sceIoChstatPatched);
    MAKE_STUB(text_addr+patches.io_remove_stub[patch_index], sceIoRemovePatched);
    MAKE_STUB(text_addr+patches.io_rmdir_stub[patch_index], sceIoRmdirPatched);
    if(me_fw) {
        MAKE_STUB(text_addr+patches.io_dopen_stub[patch_index], sceIoDopenPatched);
    }

    if(config.mode == MODE_FOLDER) {
        MAKE_STUB(text_addr+patches.io_dclose_stub[patch_index], sceIoDclosePatched);
    }
    //MAKE_STUB(text_addr+patches.io_open_stub, sceIoOpenPatched);

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
