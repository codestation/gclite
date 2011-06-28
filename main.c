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
#include "game_categories_light.h"
#include "pspdefs.h"
#include "logger.h"

PSP_MODULE_INFO("Game_Categories_Lite", 0x0007, 1, 0);
PSP_NO_CREATE_MAIN_THREAD();

/* Global variables */
GCPatches *PATCHES;
STMOD_HANDLER previous;
int game_plug = 0;

int OnModuleStart(SceModule2 *mod)
{
    kprintf("loading %s\n", mod->modname);
	if (sce_paf_private_strcmp(mod->modname, "game_plugin_module") == 0) {
	    kprintf("game_plugin_module text_addr: %08X\n", mod->text_addr);
	    game_plug = 1;
		/* Patch iofilemgr */
		PatchIoFileMgrForGamePlugin(mod->text_addr);

		/* Increase class size */
		//HijackGameClass(32);

		/* Clear the caches */
		ClearCaches();
	} else if (sce_paf_private_strcmp(mod->modname, "vsh_module") == 0) {
	    kprintf("vshmain text_addr: %08X\n", mod->text_addr);
        /* Patch muti MS system */
        PatchVshmain(mod->text_addr);
        PatchGameText(mod->text_addr);

		/* Make sceKernelGetCompiledSdkVersion clear the caches,
			so that we don't have to create a kernel module just 
			to be able to clear the caches from user mode.*/
		
		//6.20: 0xFC114573 [0x00009B0C] - SysMemUserForUser_FC114573
		//6.35:	0xFC114573 [0x000099EC] - SysMemUserForUser_FC114573
		MAKE_JUMP(PATCHES->get_compiled_sdk_version, ClearCaches);
		
		/* Clear the caches */
		ClearCaches();
	}
	return previous ? previous(mod) : 0;
}

int module_start(SceSize args, void *argp) {
    /* Determine fw group */
    u32 devkit = sceKernelDevkitVersion();
    if (devkit == 0x06020010) {
        /* Firmware(s): 6.20 */
        PATCHES = GetPatches(0);
        /* Nids changed 6.20->6.3x, resolve them (paf & vshmain isn't loaded yet :)) */
        ResolveNIDs();
    } else if (devkit >= 0x06030010 && devkit < 0x06040010) {
        /* Firmware(s): 6.3x */
        PATCHES = GetPatches(1);
    } else {
        /* Unsupported firmware */
        return 1;
    }
    /* Set start module handler */
    previous = sctrlHENSetStartModuleHandler(OnModuleStart);
    return 0;
}
