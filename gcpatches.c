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
#include <string.h>
#include <stdio.h>
#include <pspmoduleinfo.h>
#include "game_categories_light.h"

extern SceModuleInfo module_info;

typedef struct
{
	u32 nid63x;
	u32 nid620;
} nid;

nid nids[] =
{
    { 0x8F95CC01, 0xE5A74996 }, // sce_paf_private_strcpy
    { 0xD38E62C6, 0x4F487FBC }, // sce_paf_private_strncpy
    //{ 0x4C386F3C, 0xE8473E80 }, // sce_paf_private_sprintf
    { 0x726776D7, 0x5E7610DF }, // sce_paf_private_snprintf
    { 0x1B952318, 0x4900119B }, // sce_paf_private_strcmp
    { 0x9DF5623C, 0xE00E38F8 }, // sce_paf_private_strncmp
    { 0xE281261E, 0x23C8DAB5 }, // sce_paf_private_memmove
    { 0x9E9FFBFB, 0xF0D98BD1 }, // sce_paf_private_malloc
    { 0xB0363C2E, 0xE0E8820F }, // sce_paf_private_free
    { 0x49A72E5D, 0x58189108 }, // sce_paf_private_strlen
    { 0x5612DE15, 0x0C962B6E }, // sce_paf_private_strtoul
    { 0x70082F6F, 0xCB608DE5 }, // scePafGetText
};

void ResolveNIDs() {
    u32 stub_top = (u32) module_info.stub_top;
    u32 stub_end = (u32) module_info.stub_end;

    while (stub_top < stub_end) {
        SceLibraryStubTable *stub = (SceLibraryStubTable *) stub_top;

        if (strcmp(stub->libname, "scePaf") == 0) {
            for (int i = 0; i < stub->stubcount; i++) {
                for (int x = 0; x < sizeof(nids) / sizeof(nid); x++) {
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

	0x2348C, // AddVshItem
    0x21D68, // GetBackupVshItem
    //{ 0x1631C, 0x2FF8C }, // ExecuteAction
    //0x16514, // UnloadModule

    0x21E18, // AddVshItemOffset
    0x16340, // ExecuteActionOffset
    0x16734, // UnloadModuleOffset

    0x23BE0, // sce_paf_get_text_call
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
	0x23C7C, // AddVshItem
	0x22558, // GetBackupVshItem
	//{ 0x16984, 0x30828 }, // ExecuteAction
	//0x16B7C, // UnloadModule

	0x22608, // AddVshItemOffset
	0x169A8, // ExecuteActionOffset
	0x16D9C, // UnloadModuleOffset

	0x243D0, // sce_paf_get_text_call
};

GCPatches *GetPatches(int fw_group) {
    if (fw_group == 0) {
        return &patches_620;
    } else if (fw_group == 1) {
        return &patches_63x;
    }
    return NULL;
}
