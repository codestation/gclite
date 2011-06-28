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

typedef struct
{
	u32 fw_group;

	/** main.c */
	u32 current_mode;
	u32 categorize_game;
	u32 get_compiled_sdk_version;
	
	/** io.c */
	u32 io_dopen_stub;
	u32 io_dread_stub;
	u32 io_dclose_stub;

	u32 io_open_stub;
	u32 io_getstat_stub;
	u32 io_chstat_stub;
	u32 io_remove_stub;
	u32 io_rmdir_stub;
	
	u32 snprintf_call_arg_1[2];
	u32 snprintf_call_arg_2[2];
	
	/** selection.c */
	u32 play_sound;
	u32 play_sound_call;
	
	u32 array_index;
	
	u32 add_game_context_call[2];
	u32 add_game_context;
	
	u32 sce_paf_get_text_call[2];
	
	u32 setmode_call_arg_1[2];
	u32 setmode_call_arg_2[2]; // 6.20 only
	u32 setmode_arg_opcode;
	u32 setmode;
	
	u32 on_push_folder_options_call;
	u32 on_push_folder_options;
	
	u32 on_push_options_call;
	u32 on_push_options;
	
	u32 get_selection;
	u32 get_selection_arg;
	
	u32 set_selection_call[2]; // 6.30+ only
	
	int OPTION_BY_CATEGORY;
	int MODE_BY_EXPIRE_DATE;
	
	/** mode.c */
	u32 struct_addr;
	u32 index;

	/** multi.c */
	//u32 RegisterCallbacks;
	u32 AddVshItem;
	u32 GetBackupVshItem;
	u32 ExecuteAction[2];
	u32 UnloadModule;

	/* paf (multi.c) */
	u32 GetPageChild[2];
	u32 GetPageString[2];
} GCPatches;

extern GCPatches *PATCHES;
