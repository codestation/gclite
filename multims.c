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
#include "vshitem.h"
#include "psppaf.h"
#include "redirects.h"
#include "logger.h"

// from GCR v12, include/game_categories_info.h
#define game_action 0x0F

static int last_action_arg = game_action;
SceVshItem *vsh_items[2] = { NULL, NULL };

extern int game_plug;

//SceVshItem vsh_copy;
int vsh_id;
int vsh_action_arg;

extern char category[52];
extern int type;

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
            kprintf("%s: changed action_arg for %s to %i\n", __func__, category, vsh_action_arg);

            *action_arg = vsh_action_arg;
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
    if(item->id >= 100) {
        //kprintf("%s: changing id to %i\n", __func__, vsh_copy.id);
        //item->id = vsh_copy.id;
        kprintf("%s: changing id to %i\n", __func__, vsh_id);
        item->id = vsh_id;
        return item;
    }
    return NULL;
}
