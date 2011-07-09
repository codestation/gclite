/*
 * vshitem.h
 *
 *  Created on: 03/07/2011
 *      Author: code
 */

#ifndef VSHITEM_H_
#define VSHITEM_H_

#include <stddef.h>
#include "game_categories_light.h"

#define GAME_ACTION 0x0F

extern int (*AddVshItem)(void *arg, int topitem, SceVshItem *item);
extern wchar_t* (*scePafGetText)(void *arg, char *name);
extern SceVshItem *(*GetBackupVshItem)(int topitem, u32 unk, SceVshItem *item);

SceVshItem *GetBackupVshItemPatched(int topitem, u32 unk, SceVshItem *item);
void PatchVshItem(u32 text_addr);

extern int unload;

extern int vsh_id;
extern int vsh_action_arg;
extern int last_action_arg;

#endif /* VSHITEM_H_ */
