/*
 * sysconf.c
 *
 *  Created on: 03/07/2011
 *      Author: code
 */

#include <psputils.h>
#include "game_categories_light.h"
#include "psppaf.h"
#include "logger.h"

extern int sysconf_plug;
u32 sysconf_page = 0;
SceSysconfItem *sysconf_item[2] = { NULL, NULL };
char *sysconf_str[] = {"gc0", "gc1" };

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
        sysconf_item[i]->text = sysconf_str[i];
        sysconf_item[i]->regkey = sysconf_str[i];
        //kprintf("%s: text: %s, regkey: %s\n", __func__, sysconf_item[i]->text, sysconf_item[i]->regkey);
        sysconf_item[i]->page = "page_psp_config_umd_autoboot";
        option[2] = 1;
        AddSysconfItem(option, &sysconf_item[i]);
    }
}


u32 HijackContext(void *resource, SceRcoEntry *src, char **options, int n) {
    int i;
    u32 *buffer;
    SceRcoEntry *plane, *mlist, *base, *item;
    u32 *page_param, *mlist_param, *item_param;

    page_param = (u32 *) (((u8 *) src) + src->param);
    plane = (SceRcoEntry *) (((u8 *) src) + src->first_child);
    mlist = (SceRcoEntry *) (((u8 *) plane) + plane->first_child);
    mlist_param = (u32 *) (((u8 *) mlist) + mlist->param);
    base = (SceRcoEntry *) (((u8 *) mlist) + mlist->first_child);

    buffer = sce_paf_private_malloc(20 + (base->next_entry * n));
    item = (SceRcoEntry *) (((u8 *) buffer) + 20);
    item_param = (u32 *) (((u8 *) item) + base->param);

    buffer[0] = (u32) src;
    buffer[1] = mlist->first_child;
    buffer[2] = mlist->child_count;
    buffer[3] = mlist_param[16];
    buffer[4] = mlist_param[18];

    mlist->first_child = (u32) (((u8 *) item) - ((u8 *) mlist));
    mlist->child_count = n;
    mlist_param[16] = (n + 1) + (n % 2);
    mlist_param[18] = (n / 2) + (n % 2);

    for (i = 0; i < n; i++) {
        sce_paf_private_memcpy(item, base, base->next_entry);

        item_param[0] = 0xDEAD;
        item_param[1] = (u32) options[i];

        if (i != 0) {
            item->prev_entry = item->next_entry;
        }

        if (i == n - 1) {
            item->next_entry = 0;
        }

        item = (SceRcoEntry *) (((u8 *) item) + base->next_entry);
        item_param = (u32 *) (((u8 *) item) + base->param);
    }

    sceKernelDcacheWritebackAll();

    return (u32) buffer;
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

int vshGetRegistryValuePatched(u32 *option, char *name, void *arg2, int size, int *value) {
    if (name) {
        sysconf_page = ReleaseContext(sysconf_page);
        if (sce_paf_private_strcmp(name, "gc0") == 0) {
            sysconf_page = 1;
            //*value = gc_config.mode;
            return 0;
        } else if (sce_paf_private_strcmp(name, "gc1") == 0) {
            //*value = !gc_config.uncategorized;
            return 0;
        }
    }
    //return vshGetRegistryValue(option, name, arg2, size, value);
    return 0;
}

int vshSetRegistryValuePatched(u32 *option, char *name, int size,  int *value){
    if (name) {
        if (strcmp(name, "gc0") == 0) {
            //gc_config.mode = *value;
            //gcKernelSetCategorySettings(&gc_config);
            return 0;
        } else if (strcmp(name, "gc1") == 0) {
            //gc_config.uncategorized = !(*value);
            //gcKernelSetCategorySettings(&gc_config);
            return 0;
        }
    }
    //return vshSetRegistryValue(option, name, size, value);
    return 0;
}

int scePafGetPageStringPatched(void *resource, u32 *data, int *arg2, char **string, int *temp0) {
    if (data[0] == 0xDEAD) {
        //gc_utf8_to_unicode((wchar_t *) user_buffer, (char *) data[1]);
        //*(wchar_t **) string = (wchar_t *) user_buffer;
        return 0;
    }
    //return scePafGetPageString(resource, data, arg2, string, temp0);
    return 0;
}


int scePafGetPageChildPatched(void *resource, SceRcoEntry *parent, char *name, SceRcoEntry **child) {
    int res = 0;
    //int res = scePafGetPageChild(resource, parent, name, child);
    if (name) {
        if (sce_paf_private_strcmp(name, "page_psp_config_umd_autoboot") == 0) {
            if (sysconf_page == 1) {
                //sysconf_page = HijackContext(resource, *child, language_container.modes, sizeof(language_container.modes) / sizeof(char *));
            }
        } else if (strcmp(name, "page_optionmenu") == 0) {
            //PatchGetPageChildForContext(*child);
        }
    }
    return res;
}

void PatchSysconf(u32 text_addr) {
    AddSysconfItem = (void *)(U_EXTRACT_CALL(text_addr+PATCHES->AddSysconfItem));
    MAKE_CALL(text_addr+PATCHES->AddSysconfItem, AddSysconfItemPatched);

    GetSysconfItem = (void *)(U_EXTRACT_CALL(text_addr+PATCHES->GetSysconfItem));
    MAKE_CALL(text_addr+PATCHES->GetSysconfItem, GetSysconfItemPatched);
}
