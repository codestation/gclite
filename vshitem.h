/*
 *  this file is part of Game Categories Lite
 *
 *  Copyright (C) 2009, Bubbletune
 *  Copyright (C) 2011, Codestation
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

#ifndef VSHITEM_H_
#define VSHITEM_H_

#include <stddef.h>
#include "categories_lite.h"

#define GAME_ACTION 0x0F

extern int (*AddVshItem)(void *arg, int topitem, SceVshItem *item);
extern wchar_t* (*scePafGetText)(void *arg, char *name);
extern SceVshItem *(*GetBackupVshItem)(int topitem, u32 unk, SceVshItem *item);

SceVshItem *GetBackupVshItemPatched(int topitem, u32 unk, SceVshItem *item);
void PatchVshItem(u32 text_addr);

extern int unload;
extern int lang_id;
extern int vsh_id[2];
extern int vsh_action_arg[2];
extern int last_action_arg[2];

extern Category *cat_list[2];

extern int global_pos;

#endif /* VSHITEM_H_ */
