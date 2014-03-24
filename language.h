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

#ifndef LANGUAGE_H_
#define LANGUAGE_H_

typedef struct {
    char *msg_mode;
    char *msg_mode_sub;
    char *mode[3];

    char *msg_prefix;
    char *msg_prefix_sub;
    char *prefix[2];

    char *msg_show;
    char *msg_show_sub;
    char *show[4];

    char *msg_sort;
    char *msg_sort_sub;
    char *sort[2];

    char *msg_uncategorized;
    char *by_category;

} LanguageContainer;

extern LanguageContainer lang_container;

extern int lang_width[];

void LoadLanguage(int id, int location);

#endif /* LANGUAGE_H_ */
