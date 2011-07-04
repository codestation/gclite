/*
 * sysconf.c
 *
 *  Created on: 03/07/2011
 *      Author: code
 */

#include <psputils.h>
#include "game_categories_light.h"
#include "psppaf.h"

extern int sysconf_plug;
u32 sysconf_page = 0;
SceSysconfItem *sysconf_item[2] = { NULL, NULL };

void (*AddSysconfItem)(u32 *option, SceSysconfItem **item);
SceSysconfItem *(*GetSysconfItem)(void *arg0, void *arg1);

void AddSysconfItemPatched(u32 *option, SceSysconfItem **item) {
    AddSysconfItem(option, item);
    for(int i = 0; i < sizeof(sysconf_item) / 4; i++) {
        if(!sysconf_item[i]) {
            sysconf_item[i] = (SceSysconfItem *)sce_paf_private_malloc(sizeof(SceSysconfItem));
        }
        sce_paf_private_memcpy(sysconf_item[i], *item, sizeof(SceSysconfItem));
        sysconf_item[i]->id = 3;
        sysconf_item[i]->text = "gc0";
        sysconf_item[i]->regkey = "gc0";
        sysconf_item[i]->text[2] += i;
        sysconf_item[i]->regkey[2] += i;
        sysconf_item[i]->page = "page_psp_config_umd_autoboot";
        option[2] = 1;
        AddSysconfItem(option, &sysconf_item[i]);
    }
}

u32 ReleaseContext(u32 id) {
    if (id && id != 1) {
        u32 *buffer = (u32 *) id;

        if (sysconf_plug) {
            SceRcoEntry *src = (SceRcoEntry *) buffer[0];
            SceRcoEntry *plane = (SceRcoEntry *) (((u8 *) src) + src->first_child);
            SceRcoEntry *mlist = (SceRcoEntry *) (((u8 *) plane) + plane->first_child);
            u32 *mlist_param = (u32 *) (((u8 *) mlist) + mlist->param);

            mlist->first_child = buffer[1];
            mlist->child_count = buffer[2];
            mlist_param[16] = buffer[3];
            mlist_param[18] = buffer[4];
        }

        sce_paf_private_free(buffer);
        sceKernelDcacheWritebackAll();
    }

    return 0;
}

SceSysconfItem *GetSysconfItemPatched(void *arg0, void *arg1) {
    SceSysconfItem *item = GetSysconfItem(arg0, arg1);
    sysconf_page = ReleaseContext(sysconf_page);
    if (sce_paf_private_strcmp(item->text, "gc0") == 0) {
        sysconf_page = 1;
    }
    return item;
}

void PatchSysconf(u32 text_addr) {
    AddSysconfItem = (void *)(U_EXTRACT_CALL(text_addr+PATCHES->AddSysconfItem));
    MAKE_CALL(text_addr+PATCHES->AddSysconfItem, AddSysconfItemPatched);

    GetSysconfItem = (void *)(U_EXTRACT_CALL(text_addr+PATCHES->GetSysconfItem));
    MAKE_CALL(text_addr+PATCHES->GetSysconfItem, GetSysconfItemPatched);
}
