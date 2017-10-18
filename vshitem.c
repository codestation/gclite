/*
 *  this file is part of Game Categories Lite
 *
 *  Copyright (C) 2009, Bubbletune
 *  Copyright (C) 2011, Codestation
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

#include "categories_lite.h"
#include "psppaf.h"
#include "stub_funcs.h"
#include "utils.h"
#include "multims.h"
#include "context.h"
#include "gcread.h"
#include "config.h"
#include "filter.h"
#include "logger.h"
#include "language.h"
#include "utils.h"

#define GAME_ACTION 0x0F

extern int game_plug;
extern int model;
extern int context_mode;

char user_buffer[256];

int unload = 0;
int lang_id = 1;
int global_pos = 0;

Category *cat_list[2] = { NULL, NULL };

static const char* GC_PREFIX = "gc";

static const char* GC_SYSCONF_MODE = "gc0";
static const char* GC_SYSCONF_MODE_SUB = "gcs0";
static const char* GC_SYSCONF_PREFIX = "gc1";
static const char* GC_SYSCONF_PREFIX_SUB = "gcs1";
static const char* GC_SYSCONF_SHOW = "gc2";
static const char* GC_SYSCONF_SHOW_SUB = "gcs2";
static const char* GC_SYSCONF_SORT = "gc3";
static const char* GC_SYSCONF_SORT_SUB = "gcs3";

static const char* GC_UNCATEGORIZED_MS = "gc4";
static const char* GC_UNCATEGORIZED_INTERNAL = "gc5";
static const char* GC_CATEGORY_PREFIX_MS = "gcv_";
static const char* GC_CATEGORY_PREFIX_INTERNAL = "gcw_";


int vsh_id[2] = { -1, -1 };
int vsh_action_arg[2] = { -1, -1 };
int last_action_arg[2] = { GAME_ACTION, GAME_ACTION };

int (*UnloadModule)(int skip) = NULL;
int (*ExecuteAction)(int action, int action_arg) = NULL;
int (*AddVshItem)(void *arg, int topitem, SceVshItem *item) = NULL;
wchar_t* (*scePafGetText)(void *arg, const char *name) = NULL;
SceVshItem *(*GetBackupVshItem)(int topitem, u32 unk, SceVshItem *item) = NULL;
int (*sceVshCommonGuiDisplayContext_func)(void *arg, char *page, char *plane, int width, char *mlist, void *temp1, void *temp2) = NULL;

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

SceVshItem *GetBackupVshItemPatched(u32 unk, int topitem, SceVshItem *item) {
    SceVshItem *ret;
    kprintf("item: %s, topitem: %i, id: %i\n", item->text, topitem, item->id);
    SceVshItem *res = GetBackupVshItem(unk, topitem, item);
    if(config.mode == MODE_MULTI_MS) {
        if ((ret = PatchGetBackupVshItemForMultiMs(item, res))) {
            return ret;
        }
    } else if(config.mode == MODE_CONTEXT_MENU){
        PatchGetBackupVshItemForContext(item, res);
    }
    return res;
}

int AddVshItemPatched(void *arg, int topitem, SceVshItem *item) {
    int location;
    if((location = get_item_location(topitem, item)) >= 0) {
        load_config();
        load_filter();
        lang_id = get_registry_value("/CONFIG/SYSTEM/XMB", "language");
        LoadLanguage(lang_id, model == 4 ? INTERNAL_STORAGE : MEMORY_STICK);
        kprintf("got %s, location: %i, id: %i\n", item->text, location, item->id);
        category[0] = '\0';

        if (vsh_items[location]) {
            sce_paf_private_free(vsh_items[location]);
            vsh_items[location] = NULL;
        }
        if(context_items[location]) {
            sce_paf_private_free(context_items[location]);
            context_items[location] = NULL;
        }

        if(config.mode != MODE_FOLDER) {
            ClearCategories(cat_list, location);
            IndexCategories(cat_list, "xxx:/PSP/GAME", location);
        }

        // make a backup of the id and action_arg
        if(vsh_id[location] < 0 || vsh_action_arg[location] < 0) {
            vsh_id[location] = item->id;
            vsh_action_arg[location] = item->action_arg;
        } else {
            item->id = vsh_id[location];
            item->action_arg = vsh_action_arg[location];
            item->play_sound = 1;
        }
        global_pos = location;
        kprintf("saved: id: %i, action: %i\n", vsh_id[location], vsh_action_arg[location]);
        last_action_arg[location] = GAME_ACTION;

        /* Restore in case it was changed by MultiMs */
        const char *msg = location == MEMORY_STICK ? "msgshare_ms" : "msg_em";
        sce_paf_private_strcpy(item->text, msg);

        if(config.mode == MODE_MULTI_MS) {
            return PatchAddVshItemForMultiMs(arg, topitem, item, location);
        } else if(config.mode == MODE_CONTEXT_MENU) {
            return PatchAddVshItemForContext(arg, topitem, item, location);
        }
    }
    return AddVshItem(arg, topitem, item);
}

int ExecuteActionPatched(int action, int action_arg) {
    int location;
    kprintf("action: %i, action_arg: %i\n", action, action_arg);
    if(config.mode == MODE_MULTI_MS) {
        location = PatchExecuteActionForMultiMs(&action, &action_arg);
        if(location >= 0) {
            last_action_arg[location] = action_arg;
            action_arg = vsh_action_arg[location];
        }
    } else if(config.mode == MODE_CONTEXT_MENU) {
        location = PatchExecuteActionForContext(&action, &action_arg);
        if(location == 2) {
            return 0;
        } else if(location >= 0) {
            last_action_arg[location] = action_arg;
            action_arg = vsh_action_arg[location];

            // simulate MS selection
            action = GAME_ACTION;
        }
    }
    kprintf("sending action: %i, action_arg: %i\n", action, action_arg);
    return ExecuteAction(action, action_arg);
}

int UnloadModulePatched(int skip) {
    if (unload) {
        skip = -1;
        game_plug = 0;
        unload = 0;
    }
    return UnloadModule(skip);
}

wchar_t* scePafGetTextPatched(void *arg, char *name) {
    if (name && sce_paf_private_strncmp(name, GC_PREFIX, 2) == 0) {
        kprintf("match name: %s\n", name);
        //TODO: optimize this code
        // sysconf 1
        if (sce_paf_private_strcmp(name, GC_SYSCONF_MODE) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_mode);
            return (wchar_t *) user_buffer;
        // sysconf 2
        } else if (sce_paf_private_strcmp(name, GC_SYSCONF_PREFIX) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_prefix);
            return (wchar_t *) user_buffer;
        // sysconf 3
        } else if (sce_paf_private_strcmp(name, GC_SYSCONF_SHOW) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_show);
            return (wchar_t *) user_buffer;
            // sysconf 4
        } else if (sce_paf_private_strcmp(name, GC_SYSCONF_SORT) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_sort);
            return (wchar_t *) user_buffer;
            // sysconf subtitle 1
        } else if (sce_paf_private_strcmp(name, GC_SYSCONF_MODE_SUB) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_mode_sub);
            return (wchar_t *) user_buffer;
        // sysconf subtitle 2
        } else if (sce_paf_private_strcmp(name, GC_SYSCONF_PREFIX_SUB) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_prefix_sub);
            return (wchar_t *) user_buffer;
        // sysconf subtitle 3
        } else if (sce_paf_private_strcmp(name, GC_SYSCONF_SHOW_SUB) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_show_sub);
            return (wchar_t *) user_buffer;
            // sysconf subtitle 4
        } else if (sce_paf_private_strcmp(name, GC_SYSCONF_SORT_SUB) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_sort_sub);
            return (wchar_t *) user_buffer;
            // Memory Stick
        } else if (sce_paf_private_strncmp(name, GC_CATEGORY_PREFIX_MS, 4) == 0) {
            Category *p = (Category *) sce_paf_private_strtoul(name + 4, NULL, 16);
            if(config.catsort) {
                gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name+2);
            } else {
                gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name);
            }
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msgshare_ms"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        } else if (sce_paf_private_strcmp(name, GC_UNCATEGORIZED_MS) == 0) {
            gc_utf8_to_unicode((wchar_t *) user_buffer, lang_container.msg_uncategorized);
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msgshare_ms"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        // Internal Storage
        } else if (sce_paf_private_strncmp(name, GC_CATEGORY_PREFIX_INTERNAL, 4) == 0) {
            Category *p = (Category *) sce_paf_private_strtoul(name + 4, NULL, 16);
            if(config.catsort) {
                gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name+2);
            } else {
                gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name);
            }
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msg_em"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        } else if (sce_paf_private_strcmp(name, GC_UNCATEGORIZED_INTERNAL) == 0) {
            gc_utf8_to_unicode((wchar_t *) user_buffer, lang_container.msg_uncategorized);
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msg_em"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        }
    // By category (folder mode)
    } else if (name && sce_paf_private_strcmp(name, "msg_by_category") == 0) {
        gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.by_category);
        return (wchar_t *) user_buffer;
    }
    return scePafGetText(arg, name);
}


int sceVshCommonGuiDisplayContextPatched(void *arg, char *page, char *plane, int width, char *mlist, void *temp1, void *temp2) {
    if (context_gamecats || (context_mode > 0 && lang_width[lang_id])) {
        width = 1;
        context_gamecats = 0;
    }
    return sceVshCommonGuiDisplayContext_func(arg, page, plane, width, mlist, temp1, temp2);
}

void PatchVshmain(u32 text_addr) {
    AddVshItem = redir2stub(text_addr+patches.AddVshItemOffset[patch_index], add_vsh_item_stub, AddVshItemPatched);
    GetBackupVshItem = redir_call(text_addr+patches.GetBackupVshItem[patch_index], GetBackupVshItemPatched);
    ExecuteAction = redir2stub(text_addr+patches.ExecuteActionOffset[patch_index], execute_action_stub, ExecuteActionPatched);
    UnloadModule = redir2stub(text_addr+patches.UnloadModuleOffset[patch_index], unload_module_stub, UnloadModulePatched);
}

void PatchPaf(u32 text_addr) {
    //sysconf called scePafGetText from offset: 0x052AC
    scePafGetText = redir2stub(text_addr+patches.scePafGetTextOffset[patch_index], paf_get_text_stub, scePafGetTextPatched);
}

void PatchVshCommonGui(u32 text_addr) {
    sceVshCommonGuiDisplayContext_func = redir2stub(text_addr+patches.CommonGuiDisplayContextOffset[patch_index], display_context_stub, sceVshCommonGuiDisplayContextPatched);
}
