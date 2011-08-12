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
    u32 nid620;
    u32 nid630;
    u32 nid660;
} nid;

nid nids[] =
{
    { 0xE5A74996, 0x8F95CC01, 0x726DFBA9 }, // sce_paf_private_strcpy
    { 0x4F487FBC, 0xD38E62C6, 0x706ABBFF }, // sce_paf_private_strncpy
    { 0x5E7610DF, 0x726776D7, 0x7B7133D5 }, // sce_paf_private_snprintf
    { 0x4900119B, 0x1B952318, 0x4CF09BA2 }, // sce_paf_private_strcmp
    { 0xE00E38F8, 0x9DF5623C, 0xE0B32AE8 }, // sce_paf_private_strncmp
    { 0x16789955, 0x861C4627, 0xB05D9677 }, // sce_paf_private_memcmp
    { 0x23C8DAB5, 0xE281261E, 0xF7C46E37 }, // sce_paf_private_memmove
    { 0xF0D98BD1, 0x9E9FFBFB, 0x5E909060 }, // sce_paf_private_malloc
    { 0xE0E8820F, 0xB0363C2E, 0xDB1612F1 }, // sce_paf_private_free
    { 0x58189108, 0x49A72E5D, 0xD7DCB972 }, // sce_paf_private_strlen
    { 0x0C962B6E, 0x5612DE15, 0xA4B8A4E3 }, // sce_paf_private_strtoul
    { 0xF200AF8E, 0xE1C930B5, 0x02119936 }, // scePafSetSelectedItem
//    { 0xE8473E80, 0x4C386F3C, 0xA138A376 }, // sce_paf_private_sprintf
//    { 0x39E9B515, 0xBF2046E2, 0xDEADC0DE }, // scePafGetPageChild
//    { 0x62D2266B, 0x9CFBB2D9, 0xDEADC0DE }, // scePafGetPageString
//    { 0xCB608DE5, 0x70082F6F, 0xDEADC0DE }, // scePafGetText
//    { 0xE73C355B, 0x3A370539, 0xDEADC0DE }, // vshGetRegistryValue
//    { 0x2375A440, 0xCD3AF2EC, 0xDEADC0DE }, // vshSetRegistryValue
};

GCPatches patches =
{
	/** main.c */
    { 0x88009A08, 0x88009B28 }, // get_compiled_sdk_version
	
	/** gcread.c (game_plugin_module) */

	{ 0x28930, 0x2A5F0, 0x2A894 }, // io_dopen_stub
	{ 0x28940, 0x2A600, 0x2A8A4 }, // io_dread_stub
	{ 0x28948, 0x2A608, 0x2A8AC }, // io_dclose_stub

	{ 0x28958, 0x2A618, 0x2A8BC }, // io_open_stub
	{ 0x28928, 0x2A5E8, 0x2A88C }, // io_getstat_stub
	{ 0x28938, 0x2A5F8, 0x2A89C }, // io_chstat_stub
	{ 0x28950, 0x2A610, 0x2A8B4 }, // io_remove_stub
	{ 0x28960, 0x2A620, 0x2A8C4 }, // io_rmdir_stub
	
	{ 0x1FB4C, 0x21310, 0x215B4 }, // base_path
	{ 0x1FB50, 0x21314, 0x215B8 }, // base_path_arg

	{ { 0x1CD24, 0x1CD18 }, { 0x1E42C, 0x1E420 }, {0x1E6A8, 0x1E69C} }, // snprintf_call_arg_1
	{ { 0x1F8F0, 0x1F8B4 }, { 0x2109C, 0x21060 }, {0x21340, 0x21304} }, // snprintf_call_arg_2

	//{ { 0x10EFC, 0x11C68 }, { 0x1176C, 0x123A4 } }, // sce_paf_get_text_call
	
    /** vshitem.c (vshmain) */

	// { 0x1F3BC, 0x1FB70 }, // RegisterCallbacks
	{ 0x2348C, 0x23C7C, 0x23CE8 }, // AddVshItem
    { 0x21D68, 0x22558, 0x22598 }, // GetBackupVshItem
    //{ { 0x1631C, 0x2FF8C }, { 0x16984, 0x30828 } }, // ExecuteAction
    //{ 0x16514, 0x16B7C// UnloadModule

    { 0x21E18, 0x22608, 0x22648 }, // AddVshItemOffset
    { 0x16340, 0x169A8, 0x16A70 }, // ExecuteActionOffset
    { 0x16734, 0x16D9C, 0x16E64 }, // UnloadModuleOffset

    //{ 0x23BE0, 0x243D0 }, // sce_paf_get_text_call

    /** sysconf.c (sysconf_plugin_module) */
    { 0x1C4A8, 0x1CD18, 0x1D150 }, // AddSysconfItem
    { 0x02934, 0x02A28, 0x02A28 }, // GetSysconfItem

    /** sysconf.c (scePaf) */
    { 0x6750C, 0x674D4, 0x676F4 }, // GetPageNodeByIDOffset
    { 0x677EC, 0x677B4, 0x679D4 }, // ResolveRefWStringOffset

    /** sysconf.c (vshmain) */
    { 0x03908, 0x03998, 0x03998 }, // vshGetRegistryValueOffset
    { 0x03A38, 0x03AC8, 0x03AC8 }, // vshSetRegistryValueOffset

    /** vshitem.c (scePaf) */
    { 0x3C404, 0x3C3CC, 0x3C5EC }, // scePafGetTextOffset
    /** vshitem.c (commonGui) */
    { 0x03C54, 0x03C54, 0x03C54 }, // CommonGuiDisplayContextOffset

    /** context.c */
    { 0x16284, 0x168EC, 0x169B4 }, // OnXmbPush
    { 0x15D38, 0x163A0, 0x16464 }, // OnXmbContextMenu
    { 0x0DEAD, 0x0DEAD, 0x0DEAD }, // OnMenuListScrollIn
};

void ResolveNIDs(int fw_ver) {
    u32 stub_top = (u32) module_info.stub_top;
    u32 stub_end = (u32) module_info.stub_end;

    while (stub_top < stub_end) {
        SceLibraryStubTable *stub = (SceLibraryStubTable *) stub_top;
        if (strcmp(stub->libname, "scePaf") == 0) {
            for (u32 i = 0; i < stub->stubcount; i++) {
                for (u32 x = 0; x < sizeof(nids) / sizeof(nid); x++) {
                    if (stub->nidtable[i] == nids[x].nid630) {
                        if(fw_ver == FW_620) {
                            stub->nidtable[i] = nids[x].nid620;
                        } else if (fw_ver == FW_630) {
                            stub->nidtable[i] = nids[x].nid630;
                        } else if (fw_ver == FW_660) {
                            stub->nidtable[i] = nids[x].nid660;
                        }
                    }
                }
            }
        }
        stub_top += (u32)(stub->len * 4);
    }
}
