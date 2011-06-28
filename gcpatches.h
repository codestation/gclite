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

typedef struct {
	/** main.c */
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
	
	u32 base_path;
	u32 base_path_arg;

	u32 snprintf_call_arg_1[2];
	u32 snprintf_call_arg_2[2];

	u32 sce_paf_get_text_call[2];
	
	/** multi.c */
	u32 AddVshItem;
	u32 GetBackupVshItem;
	u32 ExecuteAction[2];
	u32 UnloadModule;

	u32 sce_paf_get_text;
} GCPatches;

extern GCPatches *PATCHES;
