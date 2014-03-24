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

#include <pspiofilemgr.h>
#include "config.h"
#include "psppaf.h"
#include "logger.h"

CategoryConfig config;
static CategoryConfig prev_conf = {-1, -1, -1, -1, -1};

extern int model;

char filebuf[32];

int load_config() {
    int read;
    SceUID fd;
    if(sce_paf_private_memcmp(&config, &prev_conf, sizeof(CategoryConfig)) != 0) {
        kprintf("loading config\n");
        sce_paf_private_strcpy(filebuf, "xx0:/seplugins/gclite.bin");
        SET_DEVICENAME(filebuf, model == 4 ? INTERNAL_STORAGE : MEMORY_STICK);
        if((fd = sceIoOpen(filebuf, PSP_O_RDONLY, 0777)) < 0) {
            kprintf("couldn't open gclite.bin for reading, using defaults\n");
            sce_paf_private_memset(&config, 0, sizeof(CategoryConfig));
            read = 0;
        } else {
            read = sceIoRead(fd, &config, sizeof(CategoryConfig));
            sceIoClose(fd);
        }
        sce_paf_private_memcpy(&prev_conf, &config, sizeof(CategoryConfig));
        return read == sizeof(CategoryConfig) ? 1 : 0;
    }
    return 1;
}

int save_config() {
    int written;
    SceUID fd;
    int reset;
    char device[12];

    if(sce_paf_private_memcmp(&config, &prev_conf, sizeof(CategoryConfig)) != 0) {
        if(prev_conf.mode != config.mode || prev_conf.prefix != config.prefix || prev_conf.uncategorized != config.uncategorized || prev_conf.catsort != config.catsort) {
            reset = 1;
        } else {
            reset = 0;
        }
        kprintf("saving config\n");
        sce_paf_private_strcpy(filebuf, "xx0:/seplugins/gclite.bin");
        SET_DEVICENAME(filebuf, model == 4 ? INTERNAL_STORAGE : MEMORY_STICK);
        if((fd = sceIoOpen(filebuf, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777)) < 0) {
            kprintf("couldn't open gclite.bin for writting\n");
            return 0;
        }
        written = sceIoWrite(fd, &config, sizeof(CategoryConfig));
        sceIoClose(fd);
        sce_paf_private_memcpy(&prev_conf, &config, sizeof(CategoryConfig));
        if(reset) {
            kprintf("reset MS\n");
            // Fake MS Reinsertion
            sce_paf_private_strcpy(device, "fatxx0:");
            device[3] = 'm';
            device[4] = 's';
            reset = vshIoDevctl(device, 0x0240D81E, NULL, 0, NULL, 0);
            //kprintf("vshIoDevctl for %s returned %08X\n", device, reset);
            // dunno if this works
            device[3] = 'e';
            device[4] = 'f';
            reset = vshIoDevctl(device, 0x0240D81E, NULL, 0, NULL, 0);
            //kprintf("vshIoDevctl for %s returned %08X\n", device, reset);
        }
        return written == sizeof(CategoryConfig) ? 1 : 0;
    }
    return 1;
}
