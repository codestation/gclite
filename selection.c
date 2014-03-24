/*
 Game Categories Light v 1.3
 Copyright (C) 2011, Bubbletune

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
#include <psprtc.h>
#include "psppaf.h"
#include <string.h>
#include "categories_lite.h"
#include "logger.h"

/* Global variables */
static int already_in_foldermode = 1;
extern int by_category_mode;
extern u32 text_addr_game;
static void *GetSelectionArg;
static u32 sound_call_addr;

static int defaulted;

int ToggleCategoryMode(int mode);
void HijackGameClass(int items);

/* Function pointers */
int (*AddGameContext)(void *unk, SceGameContext **item);
SceGameContext *(*GetSelection)(void *arg0, u32 arg1);
int (*SetMode)(void *arg0, void *arg1, void *arg2);
int (*OnPushFolderOptionListCascade)(void *arg0, u32 *arg1);
int (*OnPushOptionListCascade)(void *arg0, u32 *arg1);

int (*scePafSetSelection)(void *arg0, int selection);
int (*vsh_function)(void *arg);

/* Functions */
void ToggleSound(int toggle) {
    kprintf("called, toggle: %i\n", toggle);
    if (toggle == 0) {
        sound_call_addr = U_EXTRACT_CALL(text_addr_game + patches.play_sound_call[patch_index]);
        _sw(NOP_OPCODE, text_addr_game + patches.play_sound_call[patch_index]);
    }

    else {
        MAKE_CALL(text_addr_game+patches.play_sound_call[patch_index], sound_call_addr);
    }

    ClearCachesForUser();
}

int AddGameContextPatched(void *unk, SceGameContext **item) {
    /* Allocate buffer for "By Category" */
    SceGameContext *newitem = sce_paf_private_malloc(sizeof(SceGameContext));
    sce_paf_private_memcpy(newitem, *item, sizeof(SceGameContext));
    kprintf("called\n");
    /* Modify buffer for "By Category */
    newitem->text = "msg_by_category";
    newitem->option = patches.OPTION_BY_CATEGORY[patch_index];

    /* Add the option */
    AddGameContext(unk, &newitem);

    /* Add the original */
    return AddGameContext(unk, item);
}

#define MODE_ALL 0
#define OPTION_BY_EXPIRE_DATE 0

int SetModePatched(void *arg0, void *arg1, void *arg2, u32 *info) {
    /** Square-button cycling **/
    kprintf("called\n");
    /* What's the current mode? */
    if (info[patches.array_index[patch_index]] == MODE_ALL) {
        /* All */
        /* Next stop: By Expire Date */
        info[patches.array_index[patch_index]] = patches.MODE_BY_EXPIRE_DATE[patch_index];
        already_in_foldermode = 1;
    } else if (info[patches.array_index[patch_index]] == patches.MODE_BY_EXPIRE_DATE[patch_index]) {
        if (!by_category_mode) {
            /* By Expire Date */
            /* Next stop: By Category */
            ToggleCategoryMode(1);
        } else {
            /* By Category */
            /* Next stop: All */
            ToggleCategoryMode(0);
            info[patches.array_index[patch_index]] = MODE_ALL;

            already_in_foldermode = 0;
        }
    }
    return SetMode(arg0, arg1, arg2);
}

void QuickSwitchToAll(SceGameContext *selection, void *arg0, u32 *arg1) {
    kprintf("called\n");
    u32 backup = selection->option;

    /* Switch to "All" */
    selection->option = 1;

    /* Disable the click sound */
    ToggleSound(0);

    /* Call the function */
    OnPushFolderOptionListCascade(arg0, arg1);

    /* Enable the click sound */
    ToggleSound(1);

    /* Restore the original option */
    selection->option = backup;
}

int OnPushFolderOptionListCascadePatched(void *arg0, u32 *arg1) {
    kprintf("called\n");
    SceGameContext *selection = GetSelection(GetSelectionArg, arg1[3]);
    kprintf("GetSelection returned, selection: %08X\n", selection);
    /* We're in one of the folder modes in case this function is being used... */
    /* Where do we want to go? */
    if (selection->option == OPTION_BY_EXPIRE_DATE) {
        kprintf("OPTION_BY_EXPIRE_DATE\n");
        /* Check if we're already in there */
        if (by_category_mode) {
            kprintf("toggle category to 0\n");
            /* Toggle "By Category" off */
            ToggleCategoryMode(0);

            /* We're not... */
            if (already_in_foldermode) {
                kprintf("calling QuickSwitchToAll #1\n");
                /* Switch to "All" for a brief second */
                QuickSwitchToAll(selection, arg0, arg1);
            } else {
                /* We are now! */
                already_in_foldermode = 1;
            }
        }
    }

    else if (selection->option == patches.OPTION_BY_CATEGORY[patch_index]) {
        int res;
        kprintf("OPTION_BY_CATEGORY\n");
        /* Check if we're already in there */
        if (!by_category_mode) {
            /* We're not... */
            if (already_in_foldermode) {
                kprintf("calling QuickSwitchToAll #2\n");
                /* Switch to "All" for a brief second */
                QuickSwitchToAll(selection, arg0, arg1);
            } else {
                /* We are now! */
                already_in_foldermode = 1;
            }
            kprintf("toggle category to 1\n");
            /* Toggle "By Category" on */
            ToggleCategoryMode(1);

            /** HINT: This mode doesn't *really* exist, so we need to simulate "By Expire Date" */
            /* Switch to "By Expire Date" */
            selection->option = OPTION_BY_EXPIRE_DATE;
            kprintf("calling OnPushOptionListCascade\n");
            /* Call the function */
            res = OnPushOptionListCascade(arg0, arg1);

            /* Restore the original */
            selection->option = patches.OPTION_BY_CATEGORY[patch_index];
        } else {
            /* We are... */
            /* But darn, you user! You opened a lame context menu! ): */
            /* Now we need to fake another item just to make it dissappear */
            kprintf("faking item\n");
            /* Switch to "By Expire Date" */
            selection->option = OPTION_BY_EXPIRE_DATE;
            kprintf("calling OnPushFolderOptionListCascade\n");
            /* Call the function */
            res = OnPushFolderOptionListCascade(arg0, arg1);

            /* Restore the original */
            selection->option = patches.OPTION_BY_CATEGORY[patch_index];
        }

        return res;
    } else { // OPTION_ALL && OPTION_BY_FORMAT
        already_in_foldermode = 0;
    }
    kprintf("going out\n");
    return OnPushFolderOptionListCascade(arg0, arg1);
}

int OnPushOptionListCascadePatched(void *arg0, u32 *arg1) {
    kprintf("called\n");
    SceGameContext *selection = GetSelection(GetSelectionArg, arg1[3]);

    /* We're in the "All" mode in case this function is being used... */
    /* Where do we want to go? */
    if (selection->option == OPTION_BY_EXPIRE_DATE) {
        already_in_foldermode = 1;

        /* Toggle "By Category" off */
        ToggleCategoryMode(0);
    } else if (selection->option == patches.OPTION_BY_CATEGORY[patch_index]) {
        already_in_foldermode = 1;

        /* Toggle "By Category" on */
        ToggleCategoryMode(1);

        /** HINT: This mode doesn't *really* exist, so we need to simulate "By Expire Date" */
        /* Switch to "By Expire Date" */
        selection->option = OPTION_BY_EXPIRE_DATE;

        kprintf("Calling OnPushOptionListCascade\n");
        /* Call the function */
        int res = OnPushOptionListCascade(arg0, arg1);
        kprintf("Called OnPushOptionListCascade, res: %i\n", res);
        /* Restore the original */
        selection->option = patches.OPTION_BY_CATEGORY[patch_index];

        return res;
    } else {
        already_in_foldermode = 0;
    }

    return OnPushOptionListCascade(arg0, arg1);
}

int scePafSetSelectionPatched(void *arg0, int selection) {
    kprintf("called\n");
    /* "By Expire Date" */
    if (selection == 1) {
        /* Should it be "By Category"? */
        if (by_category_mode) {
            selection = 2; // yes
        }
    }

    return scePafSetSelection(arg0, selection);
}

int vsh_function_patched(void *arg) {
    if (!defaulted) {
        ToggleCategoryMode(1);

        _sw(patches.MODE_BY_EXPIRE_DATE[patch_index], text_addr_game + patches.current_mode[patch_index] + 56);
        ClearCachesForUser();

        defaulted = 1;
    }

    return vsh_function(arg);
}

void PatchSelection(u32 text_addr) {
    /* Patch AddGameContext */
    MAKE_CALL(text_addr+patches.add_game_context_call[patch_index][0], AddGameContextPatched);
    MAKE_CALL(text_addr+patches.add_game_context_call[patch_index][1], AddGameContextPatched);
    AddGameContext = (void *) (text_addr + patches.add_game_context[patch_index]);

    /* Patch calls to SetMode */
    MAKE_CALL(text_addr+patches.setmode_call_arg_1[patch_index][0], SetModePatched);
    _sw(patches.setmode_arg_opcode[patch_index], text_addr + patches.setmode_call_arg_1[patch_index][1]);

    if (patches.setmode_call_arg_2[patch_index][0]) {
        kprintf("patching SetMode\n");
        MAKE_CALL(text_addr+patches.setmode_call_arg_2[patch_index][0], SetModePatched);
        _sw(patches.setmode_arg_opcode[patch_index], text_addr + patches.setmode_call_arg_2[patch_index][1]);
    }

    /* Patch OnPushFolderOptionListCascade */
    _sw((u32) OnPushFolderOptionListCascadePatched, text_addr + patches.on_push_folder_options_call[patch_index]);
    OnPushFolderOptionListCascade = (void *) (text_addr + patches.on_push_folder_options[patch_index]);

    /* Patch OnPushOptionListCascade */
    _sw((u32) OnPushOptionListCascadePatched, text_addr + patches.on_push_options_call[patch_index]);
    OnPushOptionListCascade = (void *) (text_addr + patches.on_push_options[patch_index]);

    /* Additional function pointers */
    GetSelection = (void *) (text_addr + patches.get_selection[patch_index]);
    SetMode = (void *) (text_addr + patches.setmode[patch_index]);

    /* Argument required for GetSelection */
    GetSelectionArg = (void *) (text_addr + patches.get_selection_arg[patch_index]);

    if (patches.set_selection_call[patch_index][0]) {
        /* Patch scePafSetSelection */
        scePafSetSelection = (void *)U_EXTRACT_CALL(text_addr+patches.set_selection_call[patch_index][0]);
        MAKE_CALL(text_addr+patches.set_selection_call[patch_index][0], scePafSetSelectionPatched);
        MAKE_CALL(text_addr+patches.set_selection_call[patch_index][1], scePafSetSelectionPatched);
    }

    defaulted = 0;
    by_category_mode = 0;
    vsh_function = (void *)U_EXTRACT_CALL(text_addr+0x12E0);
    MAKE_CALL(text_addr+0x12E0, vsh_function_patched);

    HijackGameClass(32);
}
