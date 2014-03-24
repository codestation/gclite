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

#ifndef CONFIG_H_
#define CONFIG_H_

#include "categories_lite.h"

typedef struct {
    u32 mode;
    u32 prefix;
    u32 uncategorized;
    u32 selection;
    u32 catsort;
} CategoryConfig;

enum uncat {
    NONE,
    ONLY_MS,
    ONLY_IE,
    BOTH,
};

#define UNCATEGORIZED(p,u) ()!(p) && ((u) & ONLY_MS)) || (p) && ((u) & ONLY_IE))

extern CategoryConfig config;
extern char filebuf[32];

int load_config();
int save_config();

#endif /* CONFIG_H_ */
