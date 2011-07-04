	.set noreorder

	#this will be resolved to 6.20 nids if necessary by gcpatches.c
	
#include "pspstub.s"

	STUB_START "scePaf",0x00090000,0x000A0005
	STUB_FUNC  0x8F95CC01,sce_paf_private_strcpy
	STUB_FUNC  0xD38E62C6,sce_paf_private_strncpy
	//STUB_FUNC  0x4C386F3C,sce_paf_private_sprintf
	STUB_FUNC  0x726776D7,sce_paf_private_snprintf
	STUB_FUNC  0x1B952318,sce_paf_private_strcmp
	STUB_FUNC  0x9DF5623C,sce_paf_private_strncmp
	STUB_FUNC  0xE281261E,sce_paf_private_memmove
	STUB_FUNC  0x9E9FFBFB,sce_paf_private_malloc
	STUB_FUNC  0xB0363C2E,sce_paf_private_free
	STUB_FUNC  0x49A72E5D,sce_paf_private_strlen
	STUB_FUNC  0x5612DE15,sce_paf_private_strtoul
	//STUB_FUNC  0xCB608DE5,scePafGetText
	STUB_END
	
	STUB_START "Kernel_Library",0x00090000,0x00020005
	STUB_FUNC  0xA089ECA4,sce_paf_private_memset
	STUB_FUNC  0x1839852A,sce_paf_private_memcpy
	STUB_END
