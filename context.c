/*
    Game Categories v 12.0
    Copyright (C) 2009, Bubbletune

    context.c: Patches to handle Context Menu mode

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
#include <string.h>
#include <stdio.h>
#include "game_categories_light.h"
#include "psppaf.h"
#include "vshitem.h"
#include "utils.h"
#include "gcread.h"
#include "stub_funcs.h"
#include "config.h"
#include "logger.h"

extern int type;
extern int game_plug;

int (* OnXmbPush)(void *arg0, void *arg1);
int (* OnXmbContextMenu)(void *arg0, void *arg1);
int (* OnMenuListScrollIn)(void *arg0, void *arg1);

SceVshItem *original_item = NULL;
SceContextItem *original_context;
SceContextItem *context_items[2] = { NULL, NULL };

int context_gamecats = 0;
int context_just_opened = 0;
void *xmb_arg0, *xmb_arg1;

int context_action_arg;

int PatchExecuteActionForContext(int *action, int *action_arg) {
    int location;
    int uncategorized;
    kprintf("called, action: %i, action_arg: %i\n", *action, *action_arg);

    category[0] = '\0';

    location = *action_arg == 1000 ? 1 : (*action_arg == 100 ? 0 : 0);

    if (*action == GAME_ACTION && (*action_arg == 100 || *action_arg == 1000)) {
        //restore action_arg
        *action_arg = vsh_action_arg;
        context_gamecats = 1;
        original_item->context = context_items[location];
        OnXmbContextMenu(xmb_arg0, xmb_arg1);
        return 2;
    } else if (*action >= 0x80000) {

        if(game_plug) {
            if (*action_arg != last_action_arg) {
                kprintf("marking game_plugin for unload, %i != %i\n", *action_arg, last_action_arg);
                unload = 1;
            }
        }

        if(!location && (config.uncategorized & ONLY_MS)) {
            uncategorized = 1;
        } else if(!location && (config.uncategorized & ONLY_IE)) {
            uncategorized = 1;
        } else {
            uncategorized = 0;
        }

        if (!(uncategorized && *action_arg == 0)) {
            // set category
            Category *p = (Category *)sce_paf_private_strtoul(context_items[location][*action_arg].text+4, NULL, 16);
            sce_paf_private_strncpy(category, &p->name, sizeof(category));
        }

        config.selection = *action_arg;

        save_config(&config);

        return 1;
    }

    return 0;
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

    context_action_arg = item->context->action_arg;

    item->action_arg = location ? 1000 : 100;
    //last_action_arg_cnt = GAME_ACTION;

    //TODO: mark location in original context
    item->play_sound = 0;
    int malloc_size = (CountCategories(location) + i) * sizeof(SceContextItem) + 1;
    context_items[location] = sce_paf_private_malloc(malloc_size);
    sce_paf_private_memset(context_items[location], 0, malloc_size);

    if (i) {
        kprintf("creating uncategorized context\n");
        sce_paf_private_strcpy(context_items[location][0].text, "gc4");
        context_items[location][0].play_sound = 1;
        context_items[location][0].action = 0x80000;
        context_items[location][0].action_arg = 0;
    }

    type = location;

    while ((p = GetNextCategory(p, location)))
    {
        if(p->location == MEMORY_STICK) {
            sce_paf_private_snprintf(context_items[location][i].text, 48, "gcv_%08X", (u32)p);
        } else {
            sce_paf_private_snprintf(context_items[location][i].text, 48, "gcw_%08X", (u32)p);
        }
        kprintf("creating %s\n", context_items[location][i].text);
        context_items[location][i].play_sound = 1;
        context_items[location][i].action = 0x80000;
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
    if(config.mode == MODE_CONTEXT_MENU) {
        xmb_arg0 = arg0;
        xmb_arg1 = arg1;
    }
    return OnXmbPush(arg0, arg1);
}

int OnXmbContextMenuPatched(void *arg0, void *arg1) {
    kprintf("called\n");
    if(config.mode == MODE_CONTEXT_MENU) {
        context_gamecats = 0;
        if (original_item) {
            original_item->context = original_context;
        }
        sceKernelDcacheWritebackAll();
    }
    return OnXmbContextMenu(arg0, arg1);
}

void PatchGetPageChildForContext(SceRcoEntry *src) {
    kprintf("called\n");
    SceRcoEntry *plane = (SceRcoEntry *)(((u8 *)src)+src->first_child);
    SceRcoEntry *mlist = (SceRcoEntry *)(((u8 *)plane)+plane->first_child);
    u32 *mlist_param = (u32 *)(((u8 *)mlist)+mlist->param);

    if (context_gamecats)
    {
        /* Set OnMenuListScrollIn as Init function */
        mlist_param[14] = mlist_param[23];
        mlist_param[15] = mlist_param[24];

        /* Show more than only four items */
        mlist_param[13] = 0;
        mlist_param[16] = 0xC;
        mlist_param[18] = 0x6;

        context_just_opened = 1;
    }
    else
    {
        /* Restore original items */
        mlist_param[13] = 0x10;
        mlist_param[14] = 0xFFFF;
        mlist_param[15] = 0xFFFFFFFF;
        mlist_param[16] = 4;
        mlist_param[18] = 0xFFFFFFFF;
    }

    sceKernelDcacheWritebackAll();
}

void PatchGetBackupVshItemForContext(SceVshItem *item, SceVshItem *res) {
    kprintf("called\n");
    if (item->id == vsh_id) {
        original_item = res;
        original_context = item->context;
    }
}

void PatchVshmain3(u32 text_addr) {
    OnXmbPush = redir2stub(text_addr+0x168EC, xmb_push_stub, OnXmbPushPatched);
    OnXmbContextMenu = redir2stub(text_addr+0x163A0, xmb_context_stub, OnXmbContextMenuPatched);
    //OnMenuListScrollIn = redir2stub(text_addr+0x0, xmb_context_stub, OnXmbContextMenuPatched);
}
