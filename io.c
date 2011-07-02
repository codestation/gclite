/*
    Game Categories Light v 1.3
    Copyright (C) 2011, Bubbletune

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
*/

#include "game_categories_light.h"
#include "gcread.h"
#include "psppaf.h"

int sce_paf_private_snprintf_patched(char *a0, int a1, const char *a2, void *a3, void *t0) {
    sce_paf_private_strcpy((char *)a1, (char *)t0);
    return sce_paf_private_snprintf(a0, 291, a2, a3, t0);
}

void PatchIoFileMgrForGamePlugin(u32 text_addr) {

    //MAKE_STUB(text_addr+PATCHES->io_dopen_stub, sceIoDopenPatched);
    MAKE_STUB(text_addr+PATCHES->io_dread_stub, sceIoDreadPatched);
    //MAKE_STUB(text_addr+PATCHES->io_dclose_stub, sceIoDclosePatched);
    //MAKE_STUB(text_addr+PATCHES->io_open_stub, sceIoOpenPatched);
    MAKE_STUB(text_addr+PATCHES->io_getstat_stub, sceIoGetstatPatched);
    MAKE_STUB(text_addr+PATCHES->io_chstat_stub, sceIoChstatPatched);
    MAKE_STUB(text_addr+PATCHES->io_remove_stub, sceIoRemovePatched);
    MAKE_STUB(text_addr+PATCHES->io_rmdir_stub, sceIoRmdirPatched);

    MAKE_JUMP(text_addr + PATCHES->base_path, ReturnBasePathPatched);
    _sw(0x00602021, text_addr + PATCHES->base_path_arg); // move $a0, $v1

    /* SCE renames folders before removal, but it doesn't handle
        categories in doing so. It will try to move things out of
        the category with the rename function... just get rid of
        renaming to fix this lame bug. */
    // #1
    MAKE_CALL(text_addr+PATCHES->snprintf_call_arg_1[0], sce_paf_private_snprintf_patched);
    _sw(0x02402821, text_addr+PATCHES->snprintf_call_arg_1[1]); // li $a1, 291 -> move $a1, $s2

    // #2
    MAKE_CALL(text_addr+PATCHES->snprintf_call_arg_2[0], sce_paf_private_snprintf_patched);
    _sw(0x02002821, text_addr+PATCHES->snprintf_call_arg_2[1]); // li $a1, 291 -> move $a1, $s0
}

