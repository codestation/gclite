/*
 * multims.h
 *
 *  Created on: 03/07/2011
 *      Author: code
 */

#ifndef MULTIMS_H_
#define MULTIMS_H_

int PatchAddVshItemForMultiMs(void *arg, int topitem, SceVshItem *item, int location);
SceVshItem *PatchGetBackupVshItemForMultiMs(SceVshItem *item, SceVshItem *res);

int PatchExecuteActionForMultiMs(int *action, int *action_arg);

extern SceVshItem *vsh_items[2];

#endif /* MULTIMS_H_ */
