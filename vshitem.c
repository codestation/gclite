/*
 * vshitem.c
 *
 *  Created on: 03/07/2011
 *      Author: code
 */

#include "game_categories_light.h"
#include "psppaf.h"
#include "stub_funcs.h"
#include "utils.h"
#include "multims.h"
#include "context.h"
#include "gcread.h"
#include "config.h"
#include "logger.h"

char user_buffer[256];

int unload = 0;
extern int game_plug;

int (*UnloadModule)(int skip) = NULL;
int (*ExecuteAction)(int action, int action_arg) = NULL;
int (*AddVshItem)(void *arg, int topitem, SceVshItem *item) = NULL;
wchar_t* (*scePafGetText)(void *arg, char *name) = NULL;
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

// from GCR v12, user/main.c
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

// from GCR v12, user/main.c
int AddVshItemPatched(void *arg, int topitem, SceVshItem *item) {
    int location;
    if((location = get_item_location(topitem, item)) >= 0) {
        load_config(&config);
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

        ClearCategories(location);
        IndexCategories("xxx:/PSP/GAME", location);

        // make a copy of a good vsh item
        vsh_id = item->id;
        vsh_action_arg = item->action_arg;

        //kprintf(">> # action: %i\n", vsh_id);
        //kprintf(">> # action_arg: %i\n", vsh_action_arg);

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

//int sceVshCommonGuiDisplayContextPatched(void *arg, char *page, char *plane, int width, char *mlist, void *temp1, void *temp2) {
//    if (context_gamecats) {
//        width = 1;
//    }
//    return sceVshCommonGuiDisplayContext(arg, page, plane, width, mlist, temp1, temp2);
//}

// from GCR v12, user/main.c
int ExecuteActionPatched(int action, int action_arg) {
    if(config.mode == MODE_MULTI_MS) {
        PatchExecuteActionForMultiMs(&action, &action_arg);
    } else if(config.mode == MODE_CONTEXT_MENU) {
        if(PatchExecuteActionForContext(&action, &action_arg) == 2) {
            return 0;
        }
    }

    return ExecuteAction(action, action_arg);
}

// from GCR v12, user/main.c
int UnloadModulePatched(int skip) {
    if (unload) {
        skip = -1;
        game_plug = 0;
        unload = 0;
    }
    return UnloadModule(skip);
}

// based on GCR v12, user/main.c
wchar_t* scePafGetTextPatched(void *arg, char *name) {
    if (name && sce_paf_private_strncmp(name, "gc", 2) == 0) {
        kprintf("match name: %s\n", name);
        //TODO: optimize this code
        // sysconf 1
        if (sce_paf_private_strcmp(name, "gc0") == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, "Category mode");
            return (wchar_t *) user_buffer;
        // sysconf 2
        } else if (sce_paf_private_strcmp(name, "gc1") == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, "Category prefix");
            return (wchar_t *) user_buffer;
        // sysconf 3
        } else if (sce_paf_private_strcmp(name, "gc2") == 0) {
            gc_utf8_to_unicode((wchar_t *)user_buffer, "Show uncategorized");
            return (wchar_t *) user_buffer;
        // Memory Stick
        } else if (sce_paf_private_strncmp(name, "gcv_", 4) == 0) {
            Category *p = (Category *) sce_paf_private_strtoul(name + 4, NULL, 16);
            gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name);
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msgshare_ms"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        } else if (sce_paf_private_strcmp(name, "gc4") == 0) {
            gc_utf8_to_unicode((wchar_t *) user_buffer, "Uncategorized");
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msgshare_ms"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        // Internal Storage
        } else if (sce_paf_private_strncmp(name, "gcw_", 4) == 0) {
            Category *p = (Category *) sce_paf_private_strtoul(name + 4, NULL, 16);
            gc_utf8_to_unicode((wchar_t *) user_buffer, &p->name);
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msg_em"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        } else if (sce_paf_private_strcmp(name, "gc5") == 0) {
            gc_utf8_to_unicode((wchar_t *) user_buffer, "Uncategorized");
            fix_text_padding((wchar_t *) user_buffer, scePafGetText(arg, "msg_em"), 'M', 0x2122);
            return (wchar_t *) user_buffer;
        }
    }
    return scePafGetText(arg, name);
}

void PatchVshmain(u32 text_addr) {
    AddVshItem = redir2stub(text_addr+PATCHES->AddVshItemOffset, (u32)add_vsh_item_stub, AddVshItemPatched);
    GetBackupVshItem = redir_call(text_addr+PATCHES->GetBackupVshItem, GetBackupVshItemPatched);
    ExecuteAction = redir2stub(text_addr+PATCHES->ExecuteActionOffset, (u32)execute_action_stub, ExecuteActionPatched);
    UnloadModule = redir2stub(text_addr+PATCHES->UnloadModuleOffset, (u32)unload_module_stub, UnloadModulePatched);
}

void PatchPaf(u32 text_addr) {
    //sysconf called scePafGetText from offset: 0x052AC
    scePafGetText = redir2stub(text_addr+PATCHES->scePafGetTextOffset, (u32)paf_get_text_stub, scePafGetTextPatched);
}
