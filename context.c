/*
 *  this file is part of Game Categories Lite
 *
 *  Copyright (C) 2009  Bubbletune
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
#include "categories_lite.h"
#include "stub_funcs.h"
#include "psppaf.h"
#include "vshitem.h"
#include "utils.h"
#include "gcread.h"
#include "config.h"
#include "logger.h"

extern int game_plug;

int (* OnXmbPush)(void *arg0, void *arg1) = NULL;
int (* OnXmbContextMenu)(void *arg0, void *arg1) = NULL;
int (* OnMenuListScrollIn)(void *arg0, void *arg1) = NULL;

SceVshItem *original_item[2] = { NULL, NULL };
SceContextItem *original_context[2] = { NULL, NULL };
SceContextItem *context_items[2] = { NULL, NULL };

int context_gamecats = 0;
//int context_just_opened = 0;
void *xmb_arg0, *xmb_arg1;

int context_action_arg[2];

int PatchExecuteActionForContext(int *action, int *action_arg) {
    int location;
    int uncategorized;
    category[0] = '\0';

    kprintf("called, action: %i, action_arg: %i\n", *action, *action_arg);

    if (*action == GAME_ACTION && (*action_arg == 100 || *action_arg == 1000)) {
        location = get_location(*action_arg);
        global_pos = location;

        //restore action_arg
        *action_arg = vsh_action_arg[location];
        context_gamecats = 1;
        kprintf("lets hope this doesn't crash, addr: %08X\n", original_item[location]);
        original_item[location]->context = context_items[location];
        kprintf("calling OnXmbContextMenu\n");
        OnXmbContextMenu(xmb_arg0, xmb_arg1);
        kprintf("returning 2\n");
        return 2;
    } else if (*action == PSPMS_CONTEXT_SENTINEL || *action == PSPGO_CONTEXT_SENTINEL) {
        location = get_location(*action);
        global_pos = location;
        if(game_plug) {
            if (*action_arg != last_action_arg[location]) {
                kprintf("marking game_plugin for unload, %i != %i\n", *action_arg, last_action_arg[location]);
                unload = 1;
            }
        }
        kprintf("location: %i, uncategorized: %i\n", location, config.uncategorized);
        if(!location && (config.uncategorized & ONLY_MS)) {
            uncategorized = 1;
        } else if(location && (config.uncategorized & ONLY_IE)) {
            uncategorized = 1;
        } else {
            uncategorized = 0;
        }

        if (!(uncategorized && *action_arg == 0)) {
            // set category
            Category *p = (Category *)sce_paf_private_strtoul(context_items[location][*action_arg].text+4, NULL, 16);
            sce_paf_private_strncpy(category, &p->name, sizeof(category));
            kprintf("selected category: %s\n", category);
        }

        config.selection = *action_arg;

        save_config();

        return location;
    }

    return -1;
}

int PatchAddVshItemForContext(void *arg, int topitem, SceVshItem *item, int location) {
    int i;
    kprintf("called, name: %s, location: %i\n", item->text, location);
    if(!location && (config.uncategorized & ONLY_MS)) {
        i = 1;
    } else if(location && (config.uncategorized & ONLY_IE)) {
        i = 1;
    } else  {
        i = 0;
    }
    Category *p = NULL;

    context_action_arg[location] = item->context->action_arg;

    item->action_arg = location ? 1000 : 100;

    //TODO: mark location in original context
    item->play_sound = 0;
    int malloc_size = (CountCategories(location) + i) * sizeof(SceContextItem) + 1;
    context_items[location] = sce_paf_private_malloc(malloc_size);
    sce_paf_private_memset(context_items[location], 0, malloc_size);

    if (i) {
        kprintf("creating uncategorized context\n");
        sce_paf_private_strcpy(context_items[location][0].text, "gc4");
        context_items[location][0].play_sound = 1;
        context_items[location][0].action = !location ? PSPMS_CONTEXT_SENTINEL : PSPGO_CONTEXT_SENTINEL;
        context_items[location][0].action_arg = 0;
    }

    while ((p = GetNextCategory(p, location)))
    {
        if(p->location == MEMORY_STICK) {
            sce_paf_private_snprintf(context_items[location][i].text, 48, "gcv_%08X", (u32)p);
        } else {
            sce_paf_private_snprintf(context_items[location][i].text, 48, "gcw_%08X", (u32)p);
        }
        kprintf("creating %s\n", context_items[location][i].text);
        context_items[location][i].play_sound = 1;
        context_items[location][i].action = !location ? PSPMS_CONTEXT_SENTINEL : PSPGO_CONTEXT_SENTINEL;
        context_items[location][i].action_arg = i;
        i++;
    }

    if (config.selection > (i-1)) {
        config.selection = i-1;
    }
    return AddVshItem(arg, topitem, item);
}
/*
int OnMenuListScrollInPatched(void *arg0, void *arg1) {
    kprintf("called\n");
    if (context_just_opened)  {
        context_just_opened = 0;
        scePafSetSelectedItem(arg0, config.selection);
    }

    return OnMenuListScrollIn(arg0, arg1);
}*/

int OnXmbPushPatched(void *arg0, void *arg1) {
    kprintf("called\n");
    xmb_arg0 = arg0;
    xmb_arg1 = arg1;
    return OnXmbPush(arg0, arg1);
}

int OnXmbContextMenuPatched(void *arg0, void *arg1) {
    kprintf("called, global_pos: %i\n", global_pos);
    if (original_item[global_pos]) {
        context_gamecats = 0;
        original_item[global_pos]->context = original_context[global_pos];
    }
    sceKernelDcacheWritebackAll();
    return OnXmbContextMenu(arg0, arg1);
}

//void PatchGetPageChildForContext(SceRcoEntry *src) {
//    kprintf("called, globs_pos: %i\n", global_pos);
//    SceRcoEntry *plane = (SceRcoEntry *)(((u8 *)src)+src->first_child);
//    SceRcoEntry *mlist = (SceRcoEntry *)(((u8 *)plane)+plane->first_child);
//    u32 *mlist_param = (u32 *)(((u8 *)mlist)+mlist->param);
//
//    if (context_gamecats)
//    {
//        /* Set OnMenuListScrollIn as Init function */
//        mlist_param[14] = mlist_param[23];
//        mlist_param[15] = mlist_param[24];
//
//        /* Show more than only four items */
//        mlist_param[13] = 0;
//        mlist_param[16] = 0xC;
//        mlist_param[18] = 0x6;
//
//        context_just_opened = 1;
//    }
//    else
//    {
//        /* Restore original items */
//        mlist_param[13] = 0x10;
//        mlist_param[14] = 0xFFFF;
//        mlist_param[15] = 0xFFFFFFFF;
//        mlist_param[16] = 4;
//        mlist_param[18] = 0xFFFFFFFF;
//    }
//
//    sceKernelDcacheWritebackAll();
//}

void PatchGetBackupVshItemForContext(SceVshItem *item, SceVshItem *res) {
    kprintf("id: %i, action_arg: %i\n", item->id, item->action_arg);
    int location = get_location(item->action_arg);
    if(location != INVALID && item->id == vsh_id[location]) {
        global_pos = location;
        kprintf("restoring original content, loc: %i\n", location);
        original_item[location] = res;
        original_context[location] = item->context;
    }
}

void PatchVshmainForContext(u32 text_addr) {
    OnXmbPush = redir2stub(text_addr+PATCHES->OnXmbPush, xmb_push_stub, OnXmbPushPatched);
    OnXmbContextMenu = redir2stub(text_addr+PATCHES->OnXmbContextMenu, xmb_context_stub, OnXmbContextMenuPatched);
    //OnMenuListScrollIn = redir2stub(text_addr+PATCHES->OnMenuListScrollIn, menu_scroll_stub, OnMenuListScrollInPatched);
}
