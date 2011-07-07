/*
 * config.c
 *
 *  Created on: 06/07/2011
 *      Author: code
 */


#include <pspiofilemgr.h>
#include "config.h"
#include "psppaf.h"

CategoryConfig config;
CategoryConfig prev_conf = {-1, -1, -1};

int load_config(CategoryConfig *conf) {
    int read;
    SceUID fd;
    if(sce_paf_private_memcmp(conf, &prev_conf, sizeof(CategoryConfig)) != 0) {
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
    if(sce_paf_private_memcmp(conf, &prev_conf, sizeof(CategoryConfig)) != 0) {
        if((fd = sceIoOpen("ms0:/seplugins/gclite.bin", PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777)) < 0)
            return 0;
        written = sceIoWrite(fd, conf, sizeof(CategoryConfig));
        sceIoClose(fd);
        sce_paf_private_memcpy(&prev_conf, conf, sizeof(CategoryConfig));
        // Fake MS Reinsertion
        vshIoDevctl("fatms0:", 0x0240D81E, NULL, 0, NULL, 0);
        return written == sizeof(CategoryConfig) ? 1 : 0;
    }
    return 1;
}
