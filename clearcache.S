/*
    Game Categories v 12.0
    Copyright (C) 2009, Bubbletune

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

	.set	noreorder
	.set	nomacro

	.globl	ClearCaches
	.ent	ClearCaches
	
ClearCaches:
	.word 0x40088000
	.word 0x24091000
	.word 0x7D081240
	.word 0x01094804
	.word 0x4080E000
	.word 0x4080E800
	.word 0x00004021
	.word 0xBD010000
	.word 0xBD030000
	.word 0x25080040
	.word 0x1509FFFC
	.word 0x00000000
	
	.word 0x40088000
	.word 0x24090800
	.word 0x7D081180
	.word 0x01094804
	.word 0x00004021
	.word 0xBD140000
	.word 0xBD140000
	.word 0x25080040
	.word 0x1509FFFC
	.word 0x00000000
	.word 0x0000000F
	.word 0x00000000
	
	jr $ra
	nop
	
	.end	ClearCaches
