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

int sceKernelGetCompiledSdkVersion();

#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2)  & 0x03ffffff), a);
#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_STUB(a, f) {u32 addr = a; _sw(0x08000000 | (((u32)(f) & 0x0ffffffc) >> 2), addr); _sw(0, addr+4); }
#define U_EXTRACT_CALL(x) ((((u32)_lw((u32)x)) & ~0x0C000000) << 2)
#define REDIRECT_FUNCTION(a, f) { u32 address = a; _sw(0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff), address);  _sw(0, address+4); }
#define ClearCachesForUser sceKernelGetCompiledSdkVersion

typedef struct
{
	void *unknown;
	int option;
	const char *text;
} SceGameContext; 

typedef struct
{
	u32 addr;
	u32 opcode;
} ToggleCategoryPatch;

typedef struct
{
	void *next;
	u64 mtime;
	char letter;
	char name;
} Category;

typedef struct
{
	u8 unk0[104];
	char name[128];
	u32 unk1;
	char gamecode[10];
	u8 unk2[94];
	char firmware[5];
	u8 pad1[3];
	char category[3];
} SfoInfo;

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

enum
{
	OPTION_BY_EXPIRE_DATE,
	OPTION_ALL,
	// need dynamic
#if PSP_FIRMWARE_VERSION == 631
//	OPTION_BY_FORMAT,
#endif
//	OPTION_BY_CATEGORY,
};

enum
{
	MODE_ALL,
	// need dynamic
#if PSP_FIRMWARE_VERSION == 631
//	MODE_BY_FORMAT,
#endif
//	MODE_BY_EXPIRE_DATE,
//	MODE_BY_CATEGORY,
};

typedef struct
{
    u8 id; //00
    u8 type; //01
    u16 unk1; //02
    u32 label; //04
    u32 param; //08
    u32 first_child; //0c
    int child_count; //10
    u32 next_entry; // 14
    u32 prev_entry; //18
    u32 parent; //1c
    u32 unknown[2]; //20
} SceRcoEntry;

// Functions in scePaf
int scePafAddGameItems(void *unk, int count);
int scePafSetSelection(void *arg0, int selection);

int scePafGetPageChild(void *resource, SceRcoEntry *parent, char *name, SceRcoEntry **child);
int scePafGetPageString(void *resource, u32 *data, int *arg2, char **string, int *temp0);

// Functions in: selection.c
void PatchSelection(u32 text_addr);

// Functions in: multims.c
void PatchVshmain(u32 text_addr);
void PatchGameText(u32 text_addr);

// Functions in: mode.c
int ToggleCategoryMode(int mode);
void HijackGameClass();

// Functions in: io.c
void PatchIoFileMgrForGamePlugin(u32 text_addr);
void PatchIoFileMgrForVshmain(u32 text_addr);

// Functions in: category.c
int CountCategories();
void ClearCategories();
void AddCategory(char *category, u64 mtime);
Category *GetNextCategory(Category *prev);
void IndexCategories();

// Functions in: gcpatches.c
void ResolveNIDs(int fw_group);
GCPatches *GetPatches(int fw_group);

// Functions in: clearcache.s
void ClearCaches();

// vsh_module
int vsh_function(void *arg);
