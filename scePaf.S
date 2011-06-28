	.set noreorder

	#this will be resolved to 6.3x nids if necessary by gcpatches.c
	
#include "pspstub.s"

	STUB_START "scePaf",0x00090000,0x000E0005
	STUB_FUNC  0xE5A74996,sce_paf_private_strcpy
	STUB_FUNC  0xD38E62C6,sce_paf_private_strncpy
	STUB_FUNC  0x4C386F3C,sce_paf_private_sprintf
	STUB_FUNC  0x5E7610DF,sce_paf_private_snprintf
	STUB_FUNC  0x4900119B,sce_paf_private_strcmp
	STUB_FUNC  0xE00E38F8,sce_paf_private_strncmp
	STUB_FUNC  0xE281261E,sce_paf_private_memmove
	STUB_FUNC  0xF0D98BD1,sce_paf_private_malloc
	STUB_FUNC  0xE0E8820F,sce_paf_private_free
	STUB_FUNC  0x58189108,sce_paf_private_strlen
	STUB_FUNC  0xDFF7B9FF,sce_paf_private_strtoul
	STUB_FUNC  0xCB608DE5,scePafGetText
	STUB_FUNC  0xFBC4392D,scePafAddGameItems
	STUB_FUNC  0xE1C930B5,scePafSetSelection # 6.30+ only, so 6.30 nid
	STUB_FUNC  0xBF2046E2,scePafGetPageChild
	STUB_FUNC  0x9CFBB2D9,scePafGetPageString
	STUB_END
	
	STUB_START "Kernel_Library",0x00090000,0x00020005
	STUB_FUNC  0xA089ECA4,sce_paf_private_memset
	STUB_FUNC  0x1839852A,sce_paf_private_memcpy
	STUB_END
