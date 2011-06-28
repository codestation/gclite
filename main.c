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

PSP_MODULE_INFO("Game_Categories_Light", 0x0007, 1, 2);
PSP_NO_CREATE_MAIN_THREAD();

/* Global variables */
GCPatches *PATCHES;
STMOD_HANDLER previous;
u32 text_addr_game, text_size_game;
//extern int by_category_mode;
char user_buffer[256];
char currfw[5];
int defaulted;

SceUID gc_sema = -1;
int game_plug = 0;

/* Function pointers */
int (* AddGameContext)(void *unk, SceGameContext **item);
int (* SetMode)(void *arg0, void *arg1, void *arg2);
int (* CategorizeGame)(void *unk, int folder, int unk2);
int (* OnInitMS)(void *arg0, void *arg1);

int OnModuleStart(SceModule2 *mod)
{
    kprintf("loading %s\n", mod->modname);
	if (sce_paf_private_strcmp(mod->modname, "game_plugin_module") == 0)
	{
	    game_plug = 1;
		u32 text_addr = mod->text_addr;
		
		/* Initialise a number of variables */
		defaulted = 0;
		//by_category_mode = 0;
		text_addr_game = text_addr;
		text_size_game = mod->text_size;
		CategorizeGame = (void *)text_addr+PATCHES->categorize_game;
		
		/* Patch selection system */
		//PatchSelection(text_addr);
		
		/* Patch iofilemgr */
		PatchIoFileMgrForGamePlugin(text_addr);
		
		/* Patch a function for defaulting */
		//MAKE_CALL(text_addr+0x12E0, vsh_function_patched)

		/* Increase class size */
		//HijackGameClass(32);
		
		/* Clear the caches */
		ClearCaches();
	}
	
	else if (sce_paf_private_strcmp(mod->modname, "vsh_module") == 0)
	{
        /* Patch muti MS system */
        PatchVshmain(mod->text_addr);
        PatchGameText(mod->text_addr);

		/* Make sceKernelGetCompiledSdkVersion clear the caches,
			so that we don't have to create a kernel module just 
			to be able to clear the caches from user mode.*/
		
		//6.20: 0xFC114573 [0x00009B28] - SysMemUserForUser_FC114573
		//6.35:	0xFC114573 [0x000099EC] - SysMemUserForUser_FC114573
		MAKE_JUMP(PATCHES->get_compiled_sdk_version, ClearCaches);
		
		//PatchSystemControl();
		/* Clear the caches */
		ClearCaches();
	}
	if (!previous)
		return 0;
	
	return previous(mod);
}

int module_start(SceSize args, void *argp)
{
	/* Determine fw group */
	u32 devkit = sceKernelDevkitVersion();

	if (devkit == 0x06020010)
	{
		/* Firmware(s): 6.20 */
		PATCHES = GetPatches(0);
	}
	
	else if (devkit >= 0x06030010 && devkit < 0x06040010)
	{
		/* Firmware(s): 6.3x */
		PATCHES = GetPatches(1);
		
		/* Nids changed 6.20->6.3x, resolve them (paf & vshmain isn't loaded yet :)) */
		ResolveNIDs(1);
	}
	
	else
	{
		/* Unsupported firmware */
		return 1;
	}
	
	currfw[0] = ((devkit >> 24) & 0xF) + '0';
	currfw[1] = '.';
	currfw[2] = ((devkit >> 16) & 0xF) + '0';
	currfw[3] = ((devkit >> 8) & 0xF) + '0';
	currfw[4] = 0;

		/* Set start module handler */
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);
	
	return 0;
}
