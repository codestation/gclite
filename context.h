/*
 * context.h
 *
 *  Created on: 30/06/2011
 *      Author: code
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

int PatchAddVshItemForContext(void *arg, int topitem, SceVshItem *item, int location);
void PatchGetBackupVshItemForContext(SceVshItem *item, SceVshItem *res);

int PatchExecuteActionForContext(int *action, int *action_arg);

extern SceContextItem *context_items[2];
extern int context_gamecats;

#endif /* CONTEXT_H_ */
