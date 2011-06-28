
	.set noreorder
	.global vshmain_opcode_area
	.ent vshmain_gettext_stub

vshmain_gettext_stub:
	addiu $sp, $sp,-4
	j scePafGetTextPatched
	sw    $ra, 0($sp)
	lw    $ra, 0($sp)
	addiu $sp, $sp, 4
vshmain_opcode_area:
	nop   // 1st opcode from scePafGetText
	nop   // 2nd opcode from scePafGetText
	j 0x0 // jump to scePafGetText
	nop
	.end vshmain_gettext_stub
