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

	//u32 sce_paf_get_text_call[2];
	
	/** multi.c */
	u32 RegisterCallbacks;
	u32 AddVshItem;
	u32 GetBackupVshItem;
	//u32 ExecuteAction[2];
	//u32 UnloadModule;

	u32 AddVshItemOffset;
	u32 ExecuteActionOffset;
	u32 UnloadModuleOffset;

	//u32 sce_paf_get_text_call;

	/** sysconf.c */
	u32 AddSysconfItem;
	u32 GetSysconfItem;

    u32 GetPageNodeByIDOffset;
	u32 ResolveRefWStringOffset;

	u32 vshGetRegistryValueOffset;
	u32 vshSetRegistryValueOffset;

	/** vshitem.c */
	u32 scePafGetTextOffset;

	/** context.c */
	u32 OnXmbPush;
	u32 OnXmbContextMenu;
	u32 OnMenuListScrollIn;
} GCPatches;

extern GCPatches *PATCHES;

void ResolveNIDs();
GCPatches *GetPatches(int fw_group);

#endif /* GCPATCHES_H_ */
