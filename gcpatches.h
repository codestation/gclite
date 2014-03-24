/*
 *  this file is part of Game Categories Lite
 *
 *  Copyright (C) 2011  Bubbletune
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

#ifndef GCPATCHES_H_
#define GCPATCHES_H_

#include <pspsdk.h>

typedef struct {
    /** main.c (sceSystemMemoryManager) */
    u32 get_compiled_sdk_version[3];
    
    /** gcread.c (game_plugin_module) */
    u32 io_dopen_stub[3];
    u32 io_dread_stub[3];
    u32 io_dclose_stub[3];

    u32 io_open_stub[3];
    u32 io_getstat_stub[3];
    u32 io_chstat_stub[3];
    u32 io_remove_stub[3];
    u32 io_rmdir_stub[3];
    
    u32 base_path[3];
    u32 base_path_arg[3];

    u32 snprintf_call_arg_1[3][2];
    u32 snprintf_call_arg_2[3][2];

    //u32 sce_paf_get_text_call[2];
    
    /** vshitem.c (vshmain) */
    //u32 RegisterCallbacks[3];
    u32 AddVshItem[3];
    u32 GetBackupVshItem[3];
    //u32 ExecuteAction[3][2];
    //u32 UnloadModule[3];

    u32 AddVshItemOffset[3];
    u32 ExecuteActionOffset[3];
    u32 UnloadModuleOffset[3];

    //u32 sce_paf_get_text_call;

    /** sysconf.c (sysconf_plugin_module) */
    u32 AddSysconfItem[3];
    u32 GetSysconfItem[3];

    /** sysconf.c (scePaf) */
    u32 GetPageNodeByIDOffset[3];
    u32 ResolveRefWStringOffset[3];

    /** sysconf.c (vshmain) */
    u32 vshGetRegistryValueOffset[3];
    u32 vshSetRegistryValueOffset[3];

    /** vshitem.c (scePaf) */
    u32 scePafGetTextOffset[3];
    /** vshitem.c (commonGui) */
    u32 CommonGuiDisplayContextOffset[3];

    /** context.c */
    u32 OnXmbPush[3];
    u32 OnXmbContextMenu[3];
    //u32 OnMenuListScrollIn[3];

    //u32 scePafGetPageChildContext[3];

    // folder categories patches
    u32 current_mode[3];
    u32 categorize_game[3];
    //u32 play_sound[3];
    u32 play_sound_call[3];
    u32 array_index[3];
    u32 add_game_context_call[3][2];
    u32 add_game_context[3];
    u32 setmode_call_arg_1[3][2];
    u32 setmode_call_arg_2[3][2];
    u32 setmode_arg_opcode[3];
    u32 setmode[3];
    u32 on_push_folder_options_call[3];
    u32 on_push_folder_options[3];
    u32 on_push_options_call[3];
    u32 on_push_options[3];
    u32 get_selection[3];
    u32 get_selection_arg[3];
    u32 set_selection_call[3][2];
    int OPTION_BY_CATEGORY[3];
    u32 MODE_BY_EXPIRE_DATE[3];
    u32 struct_addr[3];
    u32 index[3];

} GCPatches;


enum fw_ver { FW_620, FW_630, FW_660};

extern GCPatches patches;
extern int patch_index;

void ResolveNIDs();
GCPatches *GetPatches(int fw_group);

#endif /* GCPATCHES_H_ */
