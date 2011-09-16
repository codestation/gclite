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
#include "psppaf.h"
#include "utils.h"
#include "config.h"
#include "logger.h"

extern int model;

static char *filter_data = NULL;
static SceSize filter_size;
static int counter;

int check_filter(const char *str) {
    int c = counter;
    char *buf = filter_data;

    if(filter_data == NULL) {
        return 0;
    }
    kprintf("checking <<%s>> in filter\n", str);
    while(c) {
        while(!*buf) {
            buf++;
        }
        kprintf("veryfing %s\n", buf);
        if(sce_paf_private_strcmp(buf, str) == 0) {
            kprintf("match for [%s]\n", str);
            return 1;
        }
        buf += sce_paf_private_strlen(buf);
        c--;
    }
    return 0;
}

static int split_filters() {
    int count = 0;
    for(u32 i = 0; i < filter_size; i++) {
        if(filter_data[i] == '\r') {
            kprintf("founr CR\n");
            filter_data[i] = 0;
        } else if(filter_data[i] == '\n') {
            kprintf("found LF\n");
            filter_data[i] = 0;
            count++;
        }
    }
    return count;
}

void unload_filter() {
    if(filter_data != NULL) {
        sce_paf_private_free(filter_data);
        filter_data = NULL;
    }
}

int load_filter() {
    SceUID fd;

    if(filter_data != NULL) {
        kprintf("unloading previous filter\n");
        unload_filter();
    }

    kprintf("loading filter\n");
    sce_paf_private_strcpy(filebuf, "xx0:/seplugins/gclite_filter.txt");
    SET_DEVICENAME(filebuf, model == 4 ? INTERNAL_STORAGE : MEMORY_STICK);
    if((fd = sceIoOpen(filebuf, PSP_O_RDONLY, 0777)) >= 0) {
        filter_size = sceIoLseek(fd, 0, PSP_SEEK_END);
        sceIoLseek(fd, 0, PSP_SEEK_SET);
        filter_data = sce_paf_private_malloc(filter_size);
        sceIoRead(fd, filter_data, filter_size);
        sceIoClose(fd);
        counter = split_filters();
        kprintf("total filter strings: %i\n", counter);
        return 0;
    } else {
        kprintf("filter not found\n");
    }
    return -1;
}

