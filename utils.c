/*
	Game Categories v 12.0
	Copyright (C) 2009, Bubbletune

	utils.c: Patch helpers
	
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
#include <string.h>

u32 gcKernelPatchExport(SceModule *mod, char *importlib, u32 nid, void *newone)
{
    int i = 0, u;
    int entry_size = mod->ent_size;
    int entry_start = (int)mod->ent_top;

    while (i < entry_size)
    {
        SceLibraryEntryTable *entry = (SceLibraryEntryTable *)(entry_start + i);

        if (entry->libname && (strcmp(entry->libname, importlib) == 0))
        {
            u32 *table = entry->entrytable;
            int total = entry->stubcount + entry->vstubcount;

            if (total > 0)
            {
                for (u = 0; u < total; u++)
                {
                    if (table[u] == nid)
                    {
                        table[u + total] = (u32)newone;
                        return 1;
                    }
                }
            }
        }

        i += (entry->len * 4);
    }
	return 0;
}
