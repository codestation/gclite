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

#ifndef PSPPAF_H_
#define PSPPAF_H_

#include <stddef.h>
#include "categories_lite.h"

void *sce_paf_private_malloc(int);
char *sce_paf_private_strcpy(char *, const char*);
char *sce_paf_private_strncpy(char *, const char*, int);
int sce_paf_private_snprintf(char *, int, const char *, ...);
int sce_paf_private_sprintf(char *, const char *, ...);
int sce_paf_private_memcmp(const void *, const void *, int);
void *sce_paf_private_memset(void *, char, int);
void *sce_paf_private_memmove(void *, const void *, int);
void sce_paf_private_free(void *);
int sce_paf_private_strlen(const char *);
void *sce_paf_private_memcpy(void *, void *, int);
int sce_paf_private_strncmp(const char *, const char *, int);
int sce_paf_private_strcmp(const char *, const char *);
unsigned int sce_paf_private_strtoul(char *nptr, void *endptr, int base);
int scePafSetSelectedItem(void *, int);

int vshIoDevctl(const char *dev, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);

int sceVshCommonGuiDisplayContext(void *arg, char *page, char *plane, int width, char *mlist, void *temp1, void *temp2);

//wchar_t* scePafGetText(void *, char *);

//int vshGetRegistryValue(u32 *option, char *name, void *arg2, int size, int *value);
//int vshSetRegistryValue(u32 *option, char *name, int size,  int *value);

#endif /* PSPPAF_H_ */
