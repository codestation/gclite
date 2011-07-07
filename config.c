/*
 * config.c
 *
 *  Created on: 06/07/2011
 *      Author: code
 */


#include <pspiofilemgr.h>
#include "config.h"

CategoryConfig config;

int load_config(CategoryConfig *conf) {
    int read;
    SceUID fd;

    if((fd = sceIoOpen("ms0:/seplugins/gclite.bin", PSP_O_RDONLY, 0777)) < 0)
        return 0;
    read = sceIoRead(fd, conf, sizeof(CategoryConfig));
    sceIoClose(fd);
    return read == sizeof(CategoryConfig) ? 1 : 0;
}

int save_config(CategoryConfig *conf) {
    int written;
    SceUID fd;

    if((fd = sceIoOpen("ms0:/seplugins/gclite.bin", PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777)) < 0)
        return 0;
    written = sceIoWrite(fd, conf, sizeof(CategoryConfig));
    sceIoClose(fd);
    return written == sizeof(CategoryConfig) ? 1 : 0;
}
