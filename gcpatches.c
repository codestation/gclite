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

#include <pspsdk.h>
#include <pspkernel.h>
#include <string.h>
#include <stdio.h>
#include <pspmoduleinfo.h>
#include "categories_lite.h"

extern SceModuleInfo module_info;

typedef struct {
	u32 nid63x;
	u32 nid620;
	u32 nid660;
} nid;

nid nids[] =
{
    { 0x8F95CC01, 0xE5A74996, 0x726DFBA9 }, // sce_paf_private_strcpy
    { 0xD38E62C6, 0x4F487FBC, 0x706ABBFF }, // sce_paf_private_strncpy
    { 0x726776D7, 0x5E7610DF, 0x7B7133D5 }, // sce_paf_private_snprintf
    { 0x1B952318, 0x4900119B, 0x4CF09BA2 }, // sce_paf_private_strcmp
    { 0x9DF5623C, 0xE00E38F8, 0xE0B32AE8 }, // sce_paf_private_strncmp
    { 0x861C4627, 0x16789955, 0xB05D9677 }, // sce_paf_private_memcmp
    { 0xE281261E, 0x23C8DAB5, 0xF7C46E37 }, // sce_paf_private_memmove
    { 0x9E9FFBFB, 0xF0D98BD1, 0x5E909060 }, // sce_paf_private_malloc
    { 0xB0363C2E, 0xE0E8820F, 0xDB1612F1 }, // sce_paf_private_free
    { 0x49A72E5D, 0x58189108, 0xD7DCB972 }, // sce_paf_private_strlen
    { 0x5612DE15, 0x0C962B6E, 0xA4B8A4E3 }, // sce_paf_private_strtoul
    { 0xE1C930B5, 0xF200AF8E, 0x02119936 }, // scePafSetSelectedItem
    //{ 0x4C386F3C, 0xE8473E80, 0xA138A376 }, // sce_paf_private_sprintf
    //{ 0xBF2046E2, 0x39E9B515 }, // scePafGetPageChild
    //{ 0x9CFBB2D9, 0x62D2266B }, // scePafGetPageString
    //{ 0x70082F6F, 0xCB608DE5 }, // scePafGetText
    //{ 0x3A370539, 0xE73C355B }, // vshGetRegistryValue
    //{ 0xCD3AF2EC, 0x2375A440 }, // vshSetRegistryValue
};

void ResolveNIDs() {
    u32 stub_top = (u32) module_info.stub_top;
    u32 stub_end = (u32) module_info.stub_end;

    while (stub_top < stub_end) {
        SceLibraryStubTable *stub = (SceLibraryStubTable *) stub_top;
        if (strcmp(stub->libname, "scePaf") == 0) {
            for (u32 i = 0; i < stub->stubcount; i++) {
                for (u32 x = 0; x < sizeof(nids) / sizeof(nid); x++) {
                    if (stub->nidtable[i] == nids[x].nid63x) {
                        stub->nidtable[i] = nids[x].nid620;
                    }
                }
            }
        }
        stub_top += (stub->len * 4);
    }
}

GCPatches patches_620 =
{
	/** main.c */
	0x88009B28, // get_compiled_sdk_version
	
	/** io.c (game_plugin_module) */

	0x28930, // io_dopen_stub
	0x28940, // io_dread_stub
	0x28948, // io_dclose_stub
	
	0x28958, // io_open_stub
	0x28928, // io_getstat_stub
	0x28938, // io_chstat_stub
	0x28950, // io_remove_stub
	0x28960, // io_rmdir_stub

	0x1FB4C, // base_path
	0x1FB50, // base_path_arg

	{ 0x1CD24, 0x1CD18 }, // snprintf_call_arg_1
	{ 0x1F8F0, 0x1F8B4 }, // snprintf_call_arg_2

	//{ 0x10EFC, 0x11C68 }, // sce_paf_get_text_call
	
    /** multi.c (vshmain) */

	0x1F3BC, // RegisterCallbacks
	0x2348C, // AddVshItem
    0x21D68, // GetBackupVshItem
    //{ 0x1631C, 0x2FF8C }, // ExecuteAction
    //0x16514, // UnloadModule

    0x21E18, // AddVshItemOffset
    0x16340, // ExecuteActionOffset
    0x16734, // UnloadModuleOffset

    //0x23BE0, // sce_paf_get_text_call

    /** sysconf.c */
    0x1C4A8, // AddSysconfItem
    0x02934, // GetSysconfItem

    0x6750C, // GetPageNodeByIDOffset
    0x677EC, // ResolveRefWStringOffset

    0x03908, // vshGetRegistryValueOffset
    0x03A38, // vshSetRegistryValueOffset

    /** vshitem.c */
    0x3C404, // scePafGetTextOffset
    0x03C54, // CommonGuiDisplayContextOffset

    /** context.c */
    0x16284, // OnXmbPush
    0x15D38, // OnXmbContextMenu
    0x0DEAD, // OnMenuListScrollIn
};

GCPatches patches_63x =
{
	/** main.c */
	0x88009A08, // get_compiled_sdk_version
	
	/** io.c (game_plugin_module) */

	0x2A5F0, // io_dopen_stub
	0x2A600, // io_dread_stub
	0x2A608, // io_dclose_stub
	
	0x2A618, // io_open_stub
	0x2A5E8, // io_getstat_stub
	0x2A5F8, // io_chstat_stub
	0x2A610, // io_remove_stub
	0x2A620, // io_rmdir_stub

    0x21310, // base_path
    0x21314, // base_path_arg

	{ 0x1E42C, 0x1E420 }, // snprintf_call_arg_1
	{ 0x2109C, 0x21060 }, // snprintf_call_arg_2

	//{ 0x1176C, 0x123A4 }, // sce_paf_get_text_call
	
	/** multi.c (vshmain) */
	0x1FB70, // RegisterCallbacks
	0x23C7C, // AddVshItem
	0x22558, // GetBackupVshItem
	//{ 0x16984, 0x30828 }, // ExecuteAction
	//0x16B7C, // UnloadModule

	0x22608, // AddVshItemOffset
	0x169A8, // ExecuteActionOffset
	0x16D9C, // UnloadModuleOffset

	//0x243D0, // sce_paf_get_text_call

    /** sysconf.c */
    0x1CD18, // AddSysconfItem
    0x02A28, // GetSysconfItem

    0x674D4, // GetPageNodeByIDOffset
    0x677B4, // ResolveRefWStringOffset

    0x03998, // vshGetRegistryValueOffset
    0x03AC8, // vshSetRegistryValueOffset

    /** vshitem.c */
    0x3C3CC, // scePafGetTextOffset
    0x03C54, // CommonGuiDisplayContextOffset

    /** context.c */
    0x168EC, // OnXmbPush
    0x163A0, // OnXmbContextMenu
    0x0DEAD, // OnMenuListScrollIn
};

GCPatches *GetPatches(int fw_group) {
    if (fw_group == 0) {
        return &patches_620;
    } else if (fw_group == 1) {
        return &patches_63x;
    }
    return NULL;
}
