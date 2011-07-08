/*
 * config.c
 *
 *  Created on: 06/07/2011
 *      Author: code
 */


#include <pspiofilemgr.h>
#include "config.h"
#include "psppaf.h"
#include "logger.h"

CategoryConfig config;
CategoryConfig prev_conf = {-1, -1, -1, -1};

int load_config(CategoryConfig *conf) {
    int read;
    SceUID fd;
    if(sce_paf_private_memcmp(conf, &prev_conf, sizeof(CategoryConfig)) != 0) {
        kprintf("loading config\n");
        if((fd = sceIoOpen("ms0:/seplugins/gclite.bin", PSP_O_RDONLY, 0777)) < 0)
            return 0;
        read = sceIoRead(fd, conf, sizeof(CategoryConfig));
        sceIoClose(fd);
        sce_paf_private_memcpy(&prev_conf, conf, sizeof(CategoryConfig));
        return read == sizeof(CategoryConfig) ? 1 : 0;
    }
    return 1;
}

int save_config(CategoryConfig *conf) {
    int written;
    SceUID fd;
    int reset;
    char device[12];

    if(sce_paf_private_memcmp(conf, &prev_conf, sizeof(CategoryConfig)) != 0) {
        if(prev_conf.mode != config.mode || prev_conf.prefix != config.prefix || prev_conf.uncategorized != config.uncategorized) {
            reset = 1;
        } else {
            reset = 0;
        }
        kprintf("saving config\n");
        if((fd = sceIoOpen("ms0:/seplugins/gclite.bin", PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777)) < 0)
            return 0;
        written = sceIoWrite(fd, conf, sizeof(CategoryConfig));
        sceIoClose(fd);
        sce_paf_private_memcpy(&prev_conf, conf, sizeof(CategoryConfig));
        if(reset) {
            kprintf("reset MS\n");
            // Fake MS Reinsertion
            sce_paf_private_strcpy(device, "fatxx0:");
            device[3] = 'm';
            device[4] = 's';
            vshIoDevctl(device, 0x0240D81E, NULL, 0, NULL, 0);
            // dunno if this works
            device[3] = 'e';
            device[4] = 'f';
            vshIoDevctl(device, 0x0240D81E, NULL, 0, NULL, 0);
        }
        return written == sizeof(CategoryConfig) ? 1 : 0;
    }
    return 1;
}
