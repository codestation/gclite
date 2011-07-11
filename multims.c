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
#include <pspiofilemgr.h>
#include "categories_lite.h"
#include "vshitem.h"
#include "psppaf.h"
#include "config.h"
#include "utils.h"
#include "logger.h"

#define GAME_ACTION 0x0F

SceVshItem *vsh_items[2] = { NULL, NULL };

extern int game_plug;

extern char category[52];
extern int type;

int PatchExecuteActionForMultiMs(int *action, int *action_arg) {
    Category *p;
    int location;

    category[0] = '\0';
    if (*action == GAME_ACTION) {
        location = get_location(*action_arg);
        if(location != INVALID) {
            *action_arg -= (location == INTERNAL_STORAGE) ? 1000 : 100;
            p = (Category *) sce_paf_private_strtoul(vsh_items[location][*action_arg].text + 4, NULL, 16);
            sce_paf_private_strncpy(category, &p->name, sizeof(category));
            kprintf("using %s as category\n", category);
            type = location;
            global_pos = location;
        } else {
            kprintf("must not happen!\n");
            location = 0;
        }

        if (game_plug) {
            if (*action_arg != last_action_arg[location]) {
                kprintf("marking game_plugin for unload, %i != %i\n", *action_arg, last_action_arg[location]);
                unload = 1;
            }
        }
        return location;
    }
    return -1;
}

int PatchAddVshItemForMultiMs(void *arg, int topitem, SceVshItem *item, int location) {
    int i = 0;
    Category *p = NULL;

    vsh_items[location] = sce_paf_private_malloc(CountCategories(location) * sizeof(SceVshItem));

    if (!location && (config.uncategorized & ONLY_MS)) {
        sce_paf_private_strcpy(item->text, "gc4");
        kprintf("adding uncategorized for Memory Stick\n");
        AddVshItem(arg, topitem, item);
    }
    if (location && (config.uncategorized & ONLY_IE)) {
        sce_paf_private_strcpy(item->text, "gc5");
        kprintf("adding uncategorized for Internal Storage\n");
        AddVshItem(arg, topitem, item);
    }

    while ((p = GetNextCategory(p, location))) {
        sce_paf_private_memcpy(&vsh_items[location][i], item, sizeof(SceVshItem));

        vsh_items[location][i].id = i + (location ? 1000 : 100);
        vsh_items[location][i].action_arg = i + (location ? 1000 : 100);
        if(p->location == MEMORY_STICK) {
            sce_paf_private_snprintf(vsh_items[location][i].text, 37, "gcv_%08X", (u32) p);
        } else {
            sce_paf_private_snprintf(vsh_items[location][i].text, 37, "gcw_%08X", (u32) p);
        }
        kprintf("adding %s for loc: %i\n", vsh_items[location][i].text, location);
        AddVshItem(arg, topitem, &vsh_items[location][i]);
        i++;
    }
    return 0;
}

SceVshItem *PatchGetBackupVshItemForMultiMs(SceVshItem *item, SceVshItem *res) {
    kprintf("text: %s, id: %i\n", item->text, item->id);
    if(item->id >= 100) {
        if(item->id >= 1000) {
            item->id = vsh_id[INTERNAL_STORAGE];
        } else {
            item->id = vsh_id[MEMORY_STICK];
        }
        return item;
    }
    return NULL;
}
