/*
 *  this file is part of Game Categories Lite
 *
 *  Copyright (C) 2011, Codestation
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

#ifndef UTILS_H_
#define UTILS_H_

void *redir2stub(u32 address, void *stub, void *redir);
void *redir_call(u32 address, void *func);
int get_location(int action_arg);
void gc_utf8_to_unicode(wchar_t *dest, const char *src);
void fix_text_padding(wchar_t *fake, wchar_t *real, wchar_t first, wchar_t last);
int get_registry_value(const char *dir, const char *name);
u64 get_mtime(const char *dir, int location);
void trim(char *str);
int GetLine(char *buf, int size, char *str);

#endif /* UTILS_H_ */
