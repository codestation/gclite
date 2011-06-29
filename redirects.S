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

	.set noreorder

	.global execute_action_stub
	.global unload_module_stub

	.global execute_action_call
	.global unload_module_call


	.ent execute_action_stub
execute_action_stub:
	nop // 1st opcode from ExecuteAction
	nop // 2nd opcode from ExecuteAction
execute_action_call:
	j 0 // jump to ExecuteAction + 8
	nop
	.end execute_action_stub

	.ent unload_module_stub
unload_module_stub:
	nop   // 1st opcode from UnloadModule
	nop   // 2nd opcode from UnloadModule
unload_module_call:
	j 0 // jump to UnloadModule + 8
	nop
	.end unload_module_stub
