/*
    Game Categories Lite 1.0
    Copyright (C) 2011, codestation

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Stub to a temporary redirect of the hooked funcs
    (to avoid conflicts with other plguins).
*/

#ifndef REDIRECTS_H_
#define REDIRECTS_H_

#include "game_categories_light.h"

void add_vsh_item_stub(void *arg, int topitem, SceVshItem *item);
void execute_action_stub(int action, int action_arg);
void unload_module_stub(int skip);
void add_vsh_item_call();
void execute_action_call();
void unload_module_call();

#endif /* REDIRECTS_H_ */
