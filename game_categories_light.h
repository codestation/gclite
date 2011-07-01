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
#include "gcpatches.h"

#ifndef GAME_CATEGORIES_LIGHT_H_
#define GAME_CATEGORIES_LIGHT_H_

int sceKernelGetCompiledSdkVersion();

#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2)  & 0x03ffffff), a);
#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_STUB(a, f) {u32 addr = a; _sw(0x08000000 | (((u32)(f) & 0x0ffffffc) >> 2), addr); _sw(0, addr+4); }
#define U_EXTRACT_CALL(x) ((((u32)_lw((u32)x)) & ~0x0C000000) << 2)
#define REDIRECT_FUNCTION(a, f) { u32 address = a; _sw(0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff), address);  _sw(0, address+4); }
#define ClearCachesForUser sceKernelGetCompiledSdkVersion

typedef struct
{
	void *next;
	int location;
	u64 mtime;
	char letter;
	char name;
} Category;

typedef struct
{
    char text[48];
    int play_sound;
    int action;
    int action_arg;
} SceContextItem; // 60

typedef struct
{
    int id; // 0
    int relocate; // 4
    int action; // 8
    int action_arg; // 12
    SceContextItem *context; // 16
    char *subtitle; // 20
    int unk; // 24
    char play_sound; // 28
    char memstick; // 29
    char umd_icon; // 30
    char image[4]; // 31
    char image_shadow[4]; // 35
    char image_glow[4]; // 39
    char text[37]; // 43
} SceVshItem; // 80

enum CategoryLocation {
    MEMORY_STICK,
    INTERNAL_STORAGE,
};

// Functions in: multims.c
void PatchVshmain(u32 text_addr);
void PatchGameText(u32 text_addr);

// Functions in: io.c
void PatchIoFileMgrForGamePlugin(u32 text_addr);

// Functions in: category.c
int CountCategories();
void ClearCategories();
void AddCategory(char *category, u64 mtime, int location);
Category *GetNextCategory(Category *prev, int location);
void IndexCategories(const char *path, int location);

// Functions in: gcpatches.c
void ResolveNIDs();
GCPatches *GetPatches(int fw_group);

// Functions in: clearcache.S
void ClearCaches();

#endif /* GAME_CATEGORIES_LIGHT_H_ */
