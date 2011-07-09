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

#ifndef GCREAD_H_
#define GCREAD_H_

#include <pspiofilemgr.h>

SceUID sceIoDopenPatched(const char *path);
int sceIoDreadPatched(SceUID fd, SceIoDirent *dir);
int sceIoGetstatPatched(char *file, SceIoStat *stat);
int sceIoChstatPatched(char *file, SceIoStat *stat, int bits);
int sceIoRemovePatched(char *file);
int sceIoRmdirPatched(char *path);
char *ReturnBasePathPatched(char *base);

extern char category[52];

#endif /* GCREAD_H_ */
