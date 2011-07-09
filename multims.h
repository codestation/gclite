/*
 *  this file is part of Game Categories Lite
 *
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

#ifndef MULTIMS_H_
#define MULTIMS_H_

int PatchAddVshItemForMultiMs(void *arg, int topitem, SceVshItem *item, int location);
SceVshItem *PatchGetBackupVshItemForMultiMs(SceVshItem *item, SceVshItem *res);

int PatchExecuteActionForMultiMs(int *action, int *action_arg);

extern SceVshItem *vsh_items[2];

#endif /* MULTIMS_H_ */
