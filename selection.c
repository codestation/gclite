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
#include <systemctrl.h>
#include "game_categories_light.h"

/* Global variables */
int already_in_foldermode = 1;
extern int by_category_mode;
extern u32 text_addr_game;
void *GetSelectionArg;

/* Function pointers */
int (* AddGameContext)(void *unk, SceGameContext **item);
SceGameContext *(* GetSelection)(void *arg0, u32 arg1);
int (* SetMode)(void *arg0, void *arg1, void *arg2);
int (* OnPushFolderOptionListCascade)(void *arg0, u32 *arg1);
int (* OnPushOptionListCascade)(void *arg0, u32 *arg1);

/* Functions */
void ToggleSound(int toggle)
{
	if (toggle == 0)
	{
		_sw(0, text_addr_game+PATCHES->play_sound_call);
	}
	
	else
	{
		MAKE_CALL(text_addr_game+PATCHES->play_sound_call, text_addr_game+PATCHES->play_sound);
	}
	
	ClearCachesForUser();
}

int AddGameContextPatched(void *unk, SceGameContext **item)
{
	/* Allocate buffer for "By Category" */
	SceGameContext *newitem = sce_paf_private_malloc(sizeof(SceGameContext));
	memcpy(newitem, *item, sizeof(SceGameContext));
	
	/* Modify buffer for "By Category */
	newitem->text = "msg_by_category";
	newitem->option = PATCHES->OPTION_BY_CATEGORY;
	
	/* Add the option */
	AddGameContext(unk, &newitem);
	
	/* Add the original */
	return AddGameContext(unk, item);
}

wchar_t* scePafGetTextPatched(void *arg, char *name)
{
	if (name)
	{
		if (sce_paf_private_strcmp(name, "msg_by_category") == 0)
		{
			return L"By Category";
		}
	}
	
	return scePafGetText(arg, name);
}

int SetModePatched(void *arg0, void *arg1, void *arg2, u32 *info)
{
	/** Square-button cycling **/

	/* What's the current mode? */
	if (info[PATCHES->array_index] == MODE_ALL)
	{
		/* All */
		/* Next stop: By Expire Date */
		info[PATCHES->array_index] = PATCHES->MODE_BY_EXPIRE_DATE;
		already_in_foldermode = 1;
	}
	
	else if (info[PATCHES->array_index] == PATCHES->MODE_BY_EXPIRE_DATE)
	{
		if (!by_category_mode)
		{
			/* By Expire Date */
			/* Next stop: By Category */
			ToggleCategoryMode(1);
		}
		
		else
		{
			/* By Category */
			/* Next stop: All */
			ToggleCategoryMode(0);
			info[PATCHES->array_index] = MODE_ALL;
			
			already_in_foldermode = 0;
		}
	}
	
	return SetMode(arg0, arg1, arg2);
}

void QuickSwitchToAll(SceGameContext *selection, void *arg0, u32 *arg1)
{
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

int OnPushFolderOptionListCascadePatched(void *arg0, u32 *arg1)
{
	SceGameContext *selection = GetSelection(GetSelectionArg, arg1[3]);
	
	/* We're in one of the folder modes in case this function is being used... */
	/* Where do we want to go? */
	if (selection->option == OPTION_BY_EXPIRE_DATE)
	{
		/* Check if we're already in there */
		if (by_category_mode)
		{
			/* Toggle "By Category" off */
			ToggleCategoryMode(0);
		
			/* We're not... */
			if (already_in_foldermode)
			{
				/* Switch to "All" for a brief second */
				QuickSwitchToAll(selection, arg0, arg1);
			}
			
			else
			{
				/* We are now! */
				already_in_foldermode = 1;
			}
		}
	}
	
	else if (selection->option == PATCHES->OPTION_BY_CATEGORY)
	{
		int res;
		
		/* Check if we're already in there */
		if (!by_category_mode)
		{
			/* We're not... */
			if (already_in_foldermode)
			{
				/* Switch to "All" for a brief second */
				QuickSwitchToAll(selection, arg0, arg1);
			}
			
			else
			{
				/* We are now! */
				already_in_foldermode = 1;
			}
			
			/* Toggle "By Category" on */
			ToggleCategoryMode(1);
			
			/** HINT: This mode doesn't *really* exist, so we need to simulate "By Expire Date" */
			/* Switch to "By Expire Date" */
			selection->option = OPTION_BY_EXPIRE_DATE;
			
			/* Call the function */
			res = OnPushOptionListCascade(arg0, arg1);
			
			/* Restore the original */
			selection->option = PATCHES->OPTION_BY_CATEGORY;
		}
		
		else
		{
			/* We are... */
			/* But darn, you user! You opened a lame context menu! ): */
			/* Now we need to fake another item just to make it dissappear */
			
			/* Switch to "By Expire Date" */
			selection->option = OPTION_BY_EXPIRE_DATE;
			
			/* Call the function */
			res = OnPushFolderOptionListCascade(arg0, arg1);
			
			/* Restore the original */
			selection->option = PATCHES->OPTION_BY_CATEGORY;
		}
		
		return res;
	}
	
	else // OPTION_ALL && OPTION_BY_FORMAT
	{
		already_in_foldermode = 0;
	}
	
	return OnPushFolderOptionListCascade(arg0, arg1);
}

int OnPushOptionListCascadePatched(void *arg0, u32 *arg1)
{
	SceGameContext *selection = GetSelection(GetSelectionArg, arg1[3]);
	
	/* We're in the "All" mode in case this function is being used... */
	/* Where do we want to go? */
	if (selection->option == OPTION_BY_EXPIRE_DATE)
	{
		already_in_foldermode = 1;
		
		/* Toggle "By Category" off */
		ToggleCategoryMode(0);
	}
		
	else if (selection->option == PATCHES->OPTION_BY_CATEGORY)
	{
		already_in_foldermode = 1;
		
		/* Toggle "By Category" on */
		ToggleCategoryMode(1);
			
		/** HINT: This mode doesn't *really* exist, so we need to simulate "By Expire Date" */
		/* Switch to "By Expire Date" */
		selection->option = OPTION_BY_EXPIRE_DATE;
		
		/* Call the function */
		int res = OnPushOptionListCascade(arg0, arg1);
		
		/* Restore the original */
		selection->option = PATCHES->OPTION_BY_CATEGORY;
		
		return res;
	}
		
	else
	{
		already_in_foldermode = 0;
	}
	
	return OnPushOptionListCascade(arg0, arg1);
}

int scePafSetSelectionPatched(void *arg0, int selection)
{
	/* "By Expire Date" */
	if (selection == 1)
	{
		/* Should it be "By Category"? */
		if (by_category_mode)
		{
			selection = 2; // yes
		}
	}

	return scePafSetSelection(arg0, selection);
}

void PatchSelection(u32 text_addr)
{
	/* Patch AddGameContext */
	MAKE_CALL(text_addr+PATCHES->add_game_context_call[0], AddGameContextPatched);
	MAKE_CALL(text_addr+PATCHES->add_game_context_call[1], AddGameContextPatched);
	AddGameContext = (void *)(text_addr+PATCHES->add_game_context);

	/* Patch scePafGetText */
	MAKE_CALL(text_addr+PATCHES->sce_paf_get_text_call[0], scePafGetTextPatched);
	MAKE_CALL(text_addr+PATCHES->sce_paf_get_text_call[1], scePafGetTextPatched);
	
	/* Patch calls to SetMode */
	MAKE_CALL(text_addr+PATCHES->setmode_call_arg_1[0], SetModePatched);
	_sw(PATCHES->setmode_arg_opcode, text_addr+PATCHES->setmode_call_arg_1[1]);
	
	if (PATCHES->setmode_call_arg_2[0])
	{
		MAKE_CALL(text_addr+PATCHES->setmode_call_arg_2[0], SetModePatched);
		_sw(PATCHES->setmode_arg_opcode, text_addr+PATCHES->setmode_call_arg_2[1]);
	}
	
	/* Patch OnPushFolderOptionListCascade */
	_sw((u32)OnPushFolderOptionListCascadePatched, text_addr+PATCHES->on_push_folder_options_call);
	OnPushFolderOptionListCascade = (void *)(text_addr+PATCHES->on_push_folder_options);
	
	/* Patch OnPushOptionListCascade */
	_sw((u32)OnPushOptionListCascadePatched, text_addr+PATCHES->on_push_options_call);
	OnPushOptionListCascade = (void *)(text_addr+PATCHES->on_push_options);
	
	/* Additional function pointers */
	GetSelection = (void *)(text_addr+PATCHES->get_selection);
	SetMode = (void *)(text_addr+PATCHES->setmode);
	
	/* Argument required for GetSelection */
	GetSelectionArg = (void *)(text_addr+PATCHES->get_selection_arg);
	
	if (PATCHES->set_selection_call[0])
	{
		/* Patch scePafSetSelection */
		MAKE_CALL(text_addr+PATCHES->set_selection_call[0], scePafSetSelectionPatched);
		MAKE_CALL(text_addr+PATCHES->set_selection_call[1], scePafSetSelectionPatched);
	}
}
