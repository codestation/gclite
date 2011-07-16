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
#include "logger.h"
#include "language.h"
#include "utils.h"

#define PICTURE_ACTION 0x0
#define AUDIO_ACTION 0x0
#define VIDEO_ACTION 0x0
#define GAME_ACTION 0x0F

extern int game_plug;
extern int model;
extern int context_mode;

int topitem_pos;

char user_buffer[256];

int unload = 0;
int lang_id = 1;
int global_pos = 0;

/**
 * Nomenclature:
 * gc: game categories
 * s: sysconf
 * p: picture
 * a: audio
 * v: video
 * g: game
 * m: memory stick
 * i: internal storage
 */

char *cat_str_sys[] = {
        "gcs0",
        "gcs1",
        "gcs2",
        "gcs3"
};

char *search_path[] = {
        "xxx:/PICTURE",
        "xxx:/MUSIC",
        "xxx:/VIDEO",
        "xxx:/PSP/GAME"
};

char *cat_unc[4][2] =  {
        { "gcp0", "gcp1" },
        { "gca0", "gca1" },
        { "gcv0", "gcv1" },
        { "gcg0", "gcg1" },
};

char *cat_str[4][2] =  {
        { "gcpm_", "gcpi_" },
        { "gcam_", "gcai_" },
        { "gcvm_", "gcvi_" },
        { "gcgm_", "gcgi_" },
};

int vsh_id[4][2] = {
        {-1, -1},
        {-1, -1},
        {-1, -1},
        {-1, -1},
};

int vsh_action_arg[4][2] = {
        {-1, -1},
        {-1, -1},
        {-1, -1},
        {-1, -1},
};

int last_action_arg[4][2] = {
        {PICTURE_ACTION, PICTURE_ACTION},
        {AUDIO_ACTION, AUDIO_ACTION},
        {VIDEO_ACTION, VIDEO_ACTION},
        { GAME_ACTION, GAME_ACTION },
};


int vsh_actions[] = { PICTURE_ACTION, AUDIO_ACTION, VIDEO_ACTION, GAME_ACTION };

int (*UnloadModule)(int skip) = NULL;
int (*ExecuteAction)(int action, int action_arg) = NULL;
int (*AddVshItem)(void *arg, int topitem, SceVshItem *item) = NULL;
wchar_t* (*scePafGetText)(void *arg, char *name) = NULL;
SceVshItem *(*GetBackupVshItem)(int topitem, u32 unk, SceVshItem *item) = NULL;
int (*sceVshCommonGuiDisplayContext_func)(void *arg, char *page, char *plane, int width, char *mlist, void *temp1, void *temp2) = NULL;

int topitem_index(int topitem) {
    if(topitem == ITEM_PICTURES) {
        return 0;
    } else if(topitem == ITEM_MUSIC) {
        return 1;
    } else if(topitem == ITEM_VIDEO) {
        return 2;
    } else if(topitem == ITEM_GAME) {
        return 3;
    }
    return -1;
}

int get_item_location(int pos, SceVshItem *item) {
    if(pos >= 0) {
        if(sce_paf_private_strcmp(item->text, "msgshare_ms") == 0 ||
                sce_paf_private_strcmp(item->text, cat_unc[pos][MEMORY_STICK]) == 0) {
            return MEMORY_STICK;
        } else if(sce_paf_private_strcmp(item->text, "msg_em") == 0 ||
                sce_paf_private_strcmp(item->text, cat_unc[pos][INTERNAL_STORAGE]) == 0) {
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
    int index = topitem_index(topitem);
    if(index >= 0 && (location = get_item_location(index, item)) >= 0) {
        kprintf("got %s, location: %i, id: %i\n", item->text, location, item->id);
        category[0] = '\0';

        if (vsh_items[index][location]) {
            sce_paf_private_free(vsh_items[index][location]);
            vsh_items[index][location] = NULL;
        }
        if(context_items[index][location]) {
            sce_paf_private_free(context_items[index][location]);
            context_items[index][location] = NULL;
        }

        topitem_pos = topitem;
        ClearCategories(location, index);
        IndexCategories(search_path[index], location, index);

        // make a backup of the id and action_arg
        if(vsh_id[index][location] < 0 || vsh_action_arg[index][location] < 0) {
            vsh_id[index][location] = item->id;
            vsh_action_arg[index][location] = item->action_arg;
        } else {
            item->id = vsh_id[index][location];
            item->action_arg = vsh_action_arg[index][location];
            item->play_sound = 1;
        }
        global_pos = location;
        kprintf("saved: id: %i, action: %i\n", vsh_id[index][location], vsh_action_arg[index][location]);
        last_action_arg[index][location] = vsh_actions[index];

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
            last_action_arg[index][location] = action_arg;
            action_arg = vsh_action_arg[index][location];
        }
    } else if(config.mode == MODE_CONTEXT_MENU) {
        location = PatchExecuteActionForContext(&action, &action_arg);
        if(location == 2) {
            return 0;
        } else if(location >= 0) {
            last_action_arg[index][location] = action_arg;
            action_arg = vsh_action_arg[index][location];

            // simulate MS selection
            action = vsh_action[index];
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
    if (name && sce_paf_private_strncmp(name, "gc", 2) == 0) {
        kprintf("match name: %s\n", name);
        //TODO: optimize this code
        // sysconf 1
        if (sce_paf_private_strcmp(name, cat_str_sys[0]) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_mode);
            return (wchar_t *) user_buffer;
        // sysconf 2
        } else if (sce_paf_private_strcmp(name, cat_str_sys[1]) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_prefix);
            return (wchar_t *) user_buffer;
        // sysconf 3
        } else if (sce_paf_private_strcmp(name, cat_str_sys[2]) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_show);
            return (wchar_t *) user_buffer;
        // sysconf 4
        } else if (sce_paf_private_strcmp(name, cat_str_sys[3]) == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, lang_container.msg_multimedia);
            return (wchar_t *) user_buffer;
        // Memory Stick
        } else if (sce_paf_private_strncmp(name, cat_str[0], 4) == 0) {
            Category *p = (Category *) sce_paf_private_strtoul(name + 4, NULL, 16);
            gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name);
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msgshare_ms"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        } else if (sce_paf_private_strcmp(name, cat_str_unc[0]) == 0) {
            gc_utf8_to_unicode((wchar_t *) user_buffer, lang_container.msg_uncategorized);
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msgshare_ms"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        // Internal Storage
        } else if (sce_paf_private_strncmp(name, cat_str[1], 4) == 0) {
            Category *p = (Category *) sce_paf_private_strtoul(name + 4, NULL, 16);
            gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name);
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msg_em"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        } else if (sce_paf_private_strcmp(name, cat_str_unc[1]) == 0) {
            gc_utf8_to_unicode((wchar_t *) user_buffer, lang_container.msg_uncategorized);
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msg_em"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        }
    }
    return scePafGetText(arg, name);
}


int sceVshCommonGuiDisplayContextPatched(void *arg, char *page, char *plane, int width, char *mlist, void *temp1, void *temp2) {
    if (context_gamecats || (context_mode > 0 && lang_width[lang_id])) {
        width = 1;
    }
    return sceVshCommonGuiDisplayContext_func(arg, page, plane, width, mlist, temp1, temp2);
}

void PatchVshmain(u32 text_addr) {
    AddVshItem = redir2stub(text_addr+PATCHES->AddVshItemOffset, add_vsh_item_stub, AddVshItemPatched);
    GetBackupVshItem = redir_call(text_addr+PATCHES->GetBackupVshItem, GetBackupVshItemPatched);
    ExecuteAction = redir2stub(text_addr+PATCHES->ExecuteActionOffset, execute_action_stub, ExecuteActionPatched);
    UnloadModule = redir2stub(text_addr+PATCHES->UnloadModuleOffset, unload_module_stub, UnloadModulePatched);
    load_config();
    lang_id = get_registry_value("/CONFIG/SYSTEM/XMB", "language");
    LoadLanguage(lang_id, model == 4 ? INTERNAL_STORAGE : MEMORY_STICK);
}

void PatchPaf(u32 text_addr) {
    //sysconf called scePafGetText from offset: 0x052AC
    scePafGetText = redir2stub(text_addr+PATCHES->scePafGetTextOffset, paf_get_text_stub, scePafGetTextPatched);
}

void PatchVshCommonGui(u32 text_addr) {
    sceVshCommonGuiDisplayContext_func = redir2stub(text_addr+PATCHES->CommonGuiDisplayContextOffset, display_context_stub, sceVshCommonGuiDisplayContextPatched);
}
