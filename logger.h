/*
 *  nploader module
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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string.h>
#include <stdio.h>

#define LOGFILE "ms0:/category.log"

#ifndef KPRINTF_ENABLED

#define kprintf(format, ...)
//#define kprintf(format, ...) printf(format, ## __VA_ARGS__)

#else

extern char buffer_log[256];

#define kprintf(format, ...) { \
    sce_paf_private_snprintf(buffer_log, 256, format, ## __VA_ARGS__); \
    kwrite(LOGFILE, buffer_log, strlen(buffer_log)); \
}

int kwrite(const char *path, void *buffer, int buflen);

#endif

#endif /* LOGGER_H_ */
