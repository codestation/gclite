/*
    Game Categories v 12.0
    Copyright (C) 2009, Bubbletune

    multims.c: Patches to handle Multiple Memsticks mode

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
#include <pspiofilemgr.h>
#include "game_categories_light.h"
#include "psppaf.h"
#include "redirects.h"
#include "logger.h"

// from GCR v12, include/game_categories_info.h
#define game_action 0x0F
#define game_action_arg 0x02
#define game_id 0x16

char user_buffer[256];

int unload = 0;
static int last_action_arg = game_action;
SceVshItem *vsh_items[2] = { NULL, NULL };

extern char category[52];
extern int game_plug;
extern int type;

int (*UnloadModule)(int skip) = NULL;
int (*ExecuteAction)(int action, int action_arg) = NULL;
int (*AddVshItem)(void *arg, int topitem, SceVshItem *item) = NULL;
wchar_t* (*scePafGetTextPatchOverride)(void *arg, char *name) = NULL;
SceVshItem *(*GetBackupVshItem)(int topitem, u32 unk, SceVshItem *item) = NULL;

int get_item_location(int topitem, SceVshItem *item) {
    /*
     * 0: sysconf
     * 1: extra (digital comics)
     * 2: pictures
     * 3: music
     * 4: videos
     * 5: games
     * 6: network
     * 7: store
     */
    if(topitem == 5) {
        if(sce_paf_private_strcmp(item->text, "msgshare_ms") == 0 ||
                sce_paf_private_strcmp(item->text, "gc4") == 0) {
            return MEMORY_STICK;
        } else if(sce_paf_private_strcmp(item->text, "msg_em") == 0 ||
                sce_paf_private_strcmp(item->text, "gc5") == 0) {
            return INTERNAL_STORAGE;
        }
    }
    return -1;
}

int PatchExecuteActionForMultiMs(int *action, int *action_arg) {
    category[0] = '\0';

    if (*action == game_action) {
        if (game_plug) {
            if (*action_arg != last_action_arg) {
                unload = 1;
            }
        }
        last_action_arg = *action_arg;

        if(*action_arg >= 100) {
            Category *p;
            if(*action_arg >= 1000) {
                kprintf("%s: action for Internal Storage called\n", __func__);
                // force the path so the Internal Storage is read (PSPGo)
                type = INTERNAL_STORAGE;
                p = (Category *) sce_paf_private_strtoul(vsh_items[INTERNAL_STORAGE][*action_arg - 1000].text + 4, NULL, 16);
            } else {
                kprintf("%s: action for Memory Stick called\n", __func__);
                // force the path so the Memory Stick is read (PSPGo)
                type = MEMORY_STICK;
                p = (Category *) sce_paf_private_strtoul(vsh_items[MEMORY_STICK][*action_arg - 100].text + 4, NULL, 16);
            }
            sce_paf_private_strncpy(category, &p->name, sizeof(category));
            kprintf("%s: changed action_arg for %s to %i\n", __func__, category, game_action_arg);
            *action_arg = game_action_arg;
        }
        return 1;
    }
    return 0;
}

int PatchAddVshItemForMultiMs(void *arg, int topitem, SceVshItem *item, int location) {
    int i = 0;
    SceIoStat stat;
    Category *p = NULL;

    vsh_items[location] = sce_paf_private_malloc(CountCategories(location) * sizeof(SceVshItem));
    sce_paf_private_memset(&stat, 0, sizeof(stat));

    // borrow the category buffer for a while
    sce_paf_private_strcpy(category, "xxx:/seplugins/hide_uncategorized.txt");
    SET_DEVICENAME(category, location);

    if (!location && sceIoGetstat(category, &stat) < 0) {
        sce_paf_private_strcpy(item->text, "gc4");
        kprintf("%s: adding uncategorized for Memory Stick\n", __func__);
        AddVshItem(arg, topitem, item);
    }
    if (location && sceIoGetstat(category, &stat) < 0) {
        sce_paf_private_strcpy(item->text, "gc5");
        kprintf("%s: adding uncategorized for Internal Storage\n", __func__);
        AddVshItem(arg, topitem, item);
    }
    // clear it again
    category[0] = '\0';

    while ((p = GetNextCategory(p, location))) {
        sce_paf_private_memcpy(&vsh_items[location][i], item, sizeof(SceVshItem));

        vsh_items[location][i].id = i + (location ? 1000 : 100);
        vsh_items[location][i].action_arg = i + (location ? 1000 : 100);
        if(p->location == MEMORY_STICK) {
            sce_paf_private_snprintf(vsh_items[location][i].text, 37, "gcv_%08X", (u32) p);
        } else {
            sce_paf_private_snprintf(vsh_items[location][i].text, 37, "gcw_%08X", (u32) p);
        }
        kprintf("%s: adding %s for loc: %i\n", __func__, vsh_items[location][i].text, location);
        AddVshItem(arg, topitem, &vsh_items[location][i]);
        i++;
    }
    return 0;
}

SceVshItem *PatchGetBackupVshItemForMultiMs(SceVshItem *item, SceVshItem *res) {
    kprintf("%s: item: %s, id: %i\n", __func__, item->text, item->id);
    if (item->id >= 100) {
        kprintf("%s: changing id to %i\n", __func__, game_id);
        item->id = game_id;
        return item;
    }
    return NULL;
}

// from GCR v12, user/main.c
SceVshItem *GetBackupVshItemPatched(u32 unk, int topitem, SceVshItem *item) {
    SceVshItem *ret;
    kprintf("%s: item: %s, topitem: %i, id: %i\n", __func__, item->text, topitem, item->id);
    SceVshItem *res = GetBackupVshItem(unk, topitem, item);
    if ((ret = PatchGetBackupVshItemForMultiMs(item, res))) {
        return ret;
    }
    return res;
}

// from GCR v12, user/main.c
int ExecuteActionPatched(int action, int action_arg) {
    PatchExecuteActionForMultiMs(&action, &action_arg);
    return ExecuteAction(action, action_arg);
}

// from GCR v12, user/main.c
int UnloadModulePatched(int skip) {
    if (unload) {
        skip = -1;
        unload = 0;
        game_plug = 0;
    }
    return UnloadModule(skip);
}

// from GCR v12, user/main.c
int AddVshItemPatched(void *arg, int topitem, SceVshItem *item) {
    int location;

    if((location = get_item_location(topitem, item)) >= 0) {

        kprintf("%s: got %s, location: %i, id: %i\n", __func__, item->text, location, item->id);
        category[0] = '\0';

        if (vsh_items[location]) {
            kprintf("%s: freeing vsh_items\n", __func__);
            sce_paf_private_free(vsh_items[location]);
            vsh_items[location] = NULL;
        }
        ClearCategories(location);
        IndexCategories("xxx:/PSP/GAME", location);

        /* Restore in case it was changed by MultiMs */
        const char *msg = location == MEMORY_STICK ? "msgshare_ms" : "msg_em";
        sce_paf_private_strcpy(item->text, msg);

        return PatchAddVshItemForMultiMs(arg, topitem, item, location);
    }
    return AddVshItem(arg, topitem, item);
}

// based on GCR v12, user/main.c
void PatchVshmain(u32 text_addr) {
    AddVshItem = (void *)text_addr+PATCHES->AddVshItemOffset;
    _sw(_lw((u32)AddVshItem), (u32)add_vsh_item_stub);
    _sw(_lw((u32)AddVshItem + 4), (u32)add_vsh_item_stub+4);
    MAKE_JUMP((u32)add_vsh_item_call, AddVshItem + 8);
    MAKE_STUB((u32)AddVshItem, AddVshItemPatched);
    AddVshItem = (void *)add_vsh_item_stub;

//    AddVshItem = (void *)(U_EXTRACT_CALL(text_addr+PATCHES->AddVshItem));
//    MAKE_CALL(text_addr+PATCHES->AddVshItem, AddVshItemPatched);

    GetBackupVshItem = (void *)(U_EXTRACT_CALL(text_addr+PATCHES->GetBackupVshItem));
    MAKE_CALL(text_addr+PATCHES->GetBackupVshItem, GetBackupVshItemPatched);

    // this direct approach conflicts with some mean plugins D:

//    ExecuteAction = (void *)(U_EXTRACT_CALL(text_addr+PATCHES->ExecuteAction[0]));
//    MAKE_CALL(text_addr+PATCHES->ExecuteAction[0], ExecuteActionPatched);
//    MAKE_CALL(text_addr+PATCHES->ExecuteAction[1], ExecuteActionPatched);
//
//    UnloadModule = (void *) (U_EXTRACT_CALL(text_addr+PATCHES->UnloadModule));
//    MAKE_CALL(text_addr+PATCHES->UnloadModule, UnloadModulePatched);

    /* since all those plugins that mess with the xmb think that is nice to overwrite
     * our patch and don't jump to it when they are done with the hook i had to make
     * a more aggresive hooking that involves some code relocation.
     */

    // get the function entry point
    ExecuteAction = (void *)text_addr+PATCHES->ExecuteActionOffset;

    // backup the first 2 opcodes into our stub
    _sw(_lw((u32)ExecuteAction), (u32)execute_action_stub);
    _sw(_lw((u32)ExecuteAction + 4), (u32)execute_action_stub+4);

    // jump from out stub to the original function
    MAKE_JUMP((u32)execute_action_call, (u32)ExecuteAction + 8);
    MAKE_STUB((u32)ExecuteAction, ExecuteActionPatched);

    // use our stub as the original entry point for the function
    ExecuteAction = (void *)execute_action_stub;

    // same as above
    UnloadModule = (void *)text_addr+PATCHES->UnloadModuleOffset;
    _sw(_lw((u32)UnloadModule), (u32)unload_module_stub);
    _sw(_lw((u32)UnloadModule + 4), (u32)unload_module_stub+4);
    MAKE_JUMP((u32)unload_module_call, UnloadModule + 8);
    MAKE_STUB((u32)UnloadModule, UnloadModulePatched);
    UnloadModule = (void *)unload_module_stub;
}

// from GCR v12, user/main.c
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

// from GCL v1.3, mode.c
void gc_utf8_to_unicode(wchar_t *dest, char *src) {
    int i;

    for (i = 0; i == 0 || src[i - 1]; i++) {
        dest[i] = src[i];
    }
}

// based on GCR v12, user/main.c
wchar_t* scePafGetTextPatched(void *arg, char *name) {
    if (name && sce_paf_private_strncmp(name, "gc", 2) == 0) {
        //kprintf("%s: name: %s\n", __func__, name);

        //TODO: optimize this code
        // Memory Stick
        if (sce_paf_private_strncmp(name, "gcv_", 4) == 0) {
            Category *p = (Category *) sce_paf_private_strtoul(name + 4, NULL, 16);
            gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name);
            fix_text_padding((wchar_t *) user_buffer, scePafGetTextPatchOverride(arg, "msgshare_ms"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        } else if (sce_paf_private_strcmp(name, "gc4") == 0) {
            gc_utf8_to_unicode((wchar_t *) user_buffer, "Uncategorized");
            fix_text_padding((wchar_t *) user_buffer, scePafGetTextPatchOverride(arg, "msgshare_ms"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        // Internal Storage
        } else if (sce_paf_private_strncmp(name, "gcw_", 4) == 0) {
            Category *p = (Category *) sce_paf_private_strtoul(name + 4, NULL, 16);
            gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name);
            fix_text_padding((wchar_t *) user_buffer, scePafGetTextPatchOverride(arg, "msg_em"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        } else if (sce_paf_private_strcmp(name, "gc5") == 0) {
            gc_utf8_to_unicode((wchar_t *) user_buffer, "Uncategorized");
            fix_text_padding((wchar_t *) user_buffer, scePafGetTextPatchOverride(arg, "msg_em"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        }
    }
    return scePafGetTextPatchOverride(arg, name);
}

void PatchGameText(u32 text_addr) {
    /* lets be nice and use address from the jal instead of a hardcoded one
     * just in case that another plugin patched the call before us
     * (i wish that the other plugins could be that nice to me too D: )
     */
    u32 offset = text_addr + PATCHES->sce_paf_get_text_call;
    scePafGetTextPatchOverride = (void *)U_EXTRACT_CALL(offset);
    MAKE_CALL(offset, scePafGetTextPatched); // gcv_* hook
    //MAKE_CALL(text_addr + 0x246D8, scePafGetText_243D0); // msgshare_info_space
}
