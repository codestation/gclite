/*
 *  this file is part of Game Categories Lite
 *
 *  Copyright (C) 2009  Bubbletune
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

#include <pspsdk.h>
#include <pspkernel.h>
#include <string.h>
#include <pspreg.h>
#include "language.h"
#include "categories_lite.h"
#include "category_lite_lang.h"
#include "psppaf.h"

char *lang[] = { "ja", "en", "fr", "es", "de", "it", "nl", "pt", "ru", "ko", "ch1", "ch2" };

LanguageContainer lang_container;

// trim and GetLine are taken from this thread:
// http://forums.mformature.net/showpost.php?p=57856&postcount=24
// information about language codes reversed from recovery.prx
// 'char' -> 'unsigned char' to avoid issues with utf-8

void trim(char *str) {
    int len = sce_paf_private_strlen(str);
    int i;

    for (i = len - 1; i >= 0; i--) {
        if (str[i] == 0x20 || str[i] == '\t') {
            str[i] = 0;
        } else {
            break;
        }
    }
}

int GetLine(char *buf, int size, char *str) {
    unsigned char ch = 0;
    int n = 0;
    int i = 0;
    unsigned char *s = (unsigned char *) str;

    while (1) {
        if (i >= size) {
            break;
        }

        ch = ((unsigned char *) buf)[i];

        if (ch < 0x20 && ch != '\t') {
            if (n != 0) {
                i++;
                break;
            }
        } else {
            *str++ = ch;
            n++;
        }

        i++;
    }

    trim((char *) s);

    return i;
}

int LoadLanguageContainer(void *data, int size) {
    int res, j;
    int i = 0;
    char *p = data;
    char line[256];

    do {
        sce_paf_private_memset(line, 0, sizeof(line));

        if ((res = GetLine(p, size, line)) > 0) {
            if (((char **) &lang_container)[i]) {
                sce_paf_private_free(((char **) &lang_container)[i]);
            }
            for (j = 0; j < sce_paf_private_strlen(line); j++) {
                if (line[j] == 0x5c)
                    line[j] = '\n';
            }

            ((char **) &lang_container)[i] = sce_paf_private_malloc(sce_paf_private_strlen(line) + 1);
            sce_paf_private_strcpy(((char **) &lang_container)[i], line);

            i++;
        }

        size -= res;
        p += res;
    } while (res > 0 && i < (sizeof(lang_container) / sizeof(char **)));

    if (i == (sizeof(lang_container) / sizeof(char **))) {
        return 1;
    }

    return 0;
}

void LoadLanguage(int id, int location) {
    int loaded = 0;

    if (id >= 0 && id < (sizeof(lang) / sizeof(char *))) {
        char path[128];
        sce_paf_private_snprintf(path, 128, "xx0:/seplugins/category_lite_%s.txt", lang[id]);
        SET_DEVICENAME(path, location);
        SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0);

        if (fd >= 0) {
            int size = sceIoLseek(fd, 0, PSP_SEEK_END);
            sceIoLseek(fd, 0, PSP_SEEK_SET);

            char *temp = (void *) sce_paf_private_malloc(size);
            sceIoRead(fd, temp, size);
            loaded = LoadLanguageContainer(temp, size);

            sce_paf_private_free(temp);
        }
    }

    if (!loaded) {
        LoadLanguageContainer(category_lite_lang, size_category_lite_lang);
    }
}
