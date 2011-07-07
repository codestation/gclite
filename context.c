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
#include "config.h"
#include "logger.h"

#define GAME_ACTION 0x0F

extern int unload;

static int last_action_arg = GAME_ACTION;

int (* OnXmbPush)(void *arg0, void *arg1);
int (* OnXmbContextMenu)(void *arg0, void *arg1);
int (* OnMenuListScrollIn)(void *arg0, void *arg1);

int (*RegisterCallbacks)(void *arg, SceCallbackItem *callbacks);

SceVshItem *original_item;
SceContextItem *original_context;
SceContextItem *context_items[2] = { NULL, NULL };

int context_gamecats = 0;
int context_just_opened = 0;
void *xmb_arg0, *xmb_arg1;

extern int vsh_id;
extern int vsh_action_arg;

extern char category[52];

extern int game_plug;

int PatchExecuteActionForContext(int *action, int *action_arg) {
    int location;
    int uncategorized;
    category[0] = '\0';
    if(*action_arg >= 100) {
        if(*action_arg >= 1000) {
            location = INTERNAL_STORAGE;
        } else {
            location = MEMORY_STICK;
        }
    } else {
        location = 0;
    }

    if (*action == GAME_ACTION && *action_arg >= 100) {
        context_gamecats = 1;
        original_item->context = context_items[location];
        OnXmbContextMenu(xmb_arg0, xmb_arg1);

        return 2;
    } else if (*action == 0x80000) {
        if(game_plug) {
            if (*action_arg != last_action_arg) {
                unload = 1;
            }
        }

        if(!location && (config.uncategorized & ONLY_MS)) {
            uncategorized = 1;
            *action_arg -= 100;
        } else if(!location && (config.uncategorized & ONLY_IE)) {
            uncategorized = 1;
            *action_arg -= 1000;
        } else {
            uncategorized = 0;
        }

        if (!(uncategorized && *action_arg == 0)) {
            Category *p = (Category *)sce_paf_private_strtoul(context_items[location][*action_arg].text+4, NULL, 16);
            sce_paf_private_strncpy(category, &p->name, sizeof(category));
        }

        config.selection = *action_arg;
        last_action_arg = *action_arg;

        *action = GAME_ACTION;
        *action_arg = vsh_action_arg;

        save_config(&config);

        return 1;
    }

    return 0;
}

int PatchAddVshItemForContext(void *arg, int topitem, SceVshItem *item, int location) {
    int i;
    kprintf("%s: called, location: %i\n", __func__, location);
    if(!location && (config.uncategorized & ONLY_MS)) {
        i = 1;
    } else if(location && (config.uncategorized & ONLY_IE)) {
        i = 1;
    } else  {
        i = 0;
    }
    Category *p = NULL;
    item->play_sound = 0;
    context_items[location] = sce_paf_private_malloc((CountCategories(location)+i)*sizeof(SceContextItem)+1);
    sce_paf_private_memset(context_items, 0, (CountCategories()+i)*sizeof(SceContextItem)+1);

    if (i) {
        kprintf("%s: creating uncategorized context\n");
        sce_paf_private_strcpy(context_items[location][0].text, "gc4");
        context_items[location][0].play_sound = 1;
        context_items[location][0].action = 0x80000;
        context_items[location][0].action_arg = (!location ? 100 : 1000);
    }

    while ((p = GetNextCategory(p, location)))
    {
        if(p->location == MEMORY_STICK) {
            sce_paf_private_snprintf(context_items[location][i].text, 48, "gcv_%08X", (u32)p);
        } else {
            sce_paf_private_snprintf(context_items[location][i].text, 48, "gcw_%08X", (u32)p);
        }
        kprintf("%s: creating %s\n", __func__, context_items[location][i].text);
        context_items[location][i].play_sound = 1;
        context_items[location][i].action = 0x80000;
        context_items[location][i].action_arg = (!location ? 100 : 1000) + i;
        i++;
    }

    if (config.selection > (i-1)) {
        config.selection = i-1;
    }
    return AddVshItem(arg, topitem, item);
}

int OnMenuListScrollInPatched(void *arg0, void *arg1)
{
    kprintf("%s\n", __func__);
    if (context_just_opened)  {
        context_just_opened = 0;
        scePafSetSelectedItem(arg0, config.selection);
    }

    return OnMenuListScrollIn(arg0, arg1);
}

int OnXmbPushPatched(void *arg0, void *arg1) {
    kprintf("%s\n", __func__);
    xmb_arg0 = arg0;
    xmb_arg1 = arg1;
    return OnXmbPush(arg0, arg1);
}

int OnXmbContextMenuPatched(void *arg0, void *arg1) {
    kprintf("%s\n", __func__);
    context_gamecats = 0;
    if (original_item) {
        original_item->context = original_context;
    }
    sceKernelDcacheWritebackAll();

    return OnXmbContextMenu(arg0, arg1);
}

int RegisterCallbacksPatched(void *arg, SceCallbackItem *callbacks) {
    int i;
    kprintf("%s\n", __func__);
    for (i = 0; callbacks[i].name && callbacks[i].callback; i++)
    {
        if (strcmp(callbacks[i].name, "OnXmbPush") == 0) {
            OnXmbPush = (void *)callbacks[i].callback;
            callbacks[i].callback = (void *)OnXmbPushPatched;
        }
        else if (strcmp(callbacks[i].name, "OnXmbContextMenu") == 0) {
            OnXmbContextMenu = (void *)callbacks[i].callback;
            callbacks[i].callback = (void *)OnXmbContextMenuPatched;
        }
        else if (strcmp(callbacks[i].name, "OnMenuListScrollIn") == 0) {
            OnMenuListScrollIn = (void *)callbacks[i].callback;
            callbacks[i].callback = (void *)OnMenuListScrollInPatched;
        }
    }

    sceKernelDcacheWritebackAll();

    return RegisterCallbacks(arg, callbacks);
}

void PatchGetPageChildForContext(SceRcoEntry *src) {
    kprintf("%s\n", __func__);
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
    kprintf("%s\n", __func__);
    if (item->id == vsh_id) {
        original_item = res;
        original_context = item->context;
    }
}

void PatchPaf3(u32 text_addr) {
    //RegisterCallbacks = redir_call(text_addr + PATCHES->RegisterCallbacks, RegisterCallbacksPatched);
}
