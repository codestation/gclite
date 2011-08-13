	.set noreorder

	#this will be resolved to 6.20 nids if necessary by gcpatches.c
	
#include "pspstub.s"

	STUB_START "scePaf",0x00090000,0x000C0005
	STUB_FUNC  0x8F95CC01,sce_paf_private_strcpy
	STUB_FUNC  0xD38E62C6,sce_paf_private_strncpy
	STUB_FUNC  0x726776D7,sce_paf_private_snprintf
	STUB_FUNC  0x1B952318,sce_paf_private_strcmp
	STUB_FUNC  0x9DF5623C,sce_paf_private_strncmp
	STUB_FUNC  0x861C4627,sce_paf_private_memcmp
	STUB_FUNC  0xE281261E,sce_paf_private_memmove
	STUB_FUNC  0x9E9FFBFB,sce_paf_private_malloc
	STUB_FUNC  0xB0363C2E,sce_paf_private_free
	STUB_FUNC  0x49A72E5D,sce_paf_private_strlen
	STUB_FUNC  0x5612DE15,sce_paf_private_strtoul
	STUB_FUNC  0xE1C930B5,scePafSetSelectedItem
	//STUB_FUNC  0x4C386F3C,sce_paf_private_sprintf
	//STUB_FUNC  0xCB608DE5,scePafGetText
	//STUB_FUNC  0xBF2046E2,scePafGetPageChild
	//STUB_FUNC  0x9CFBB2D9,scePafGetPageString
	//STUB_FUNC  0xE03A5C26,PAF_Resource_GetPageNodeByID
	//STUB_FUNC  0xFDAFC9E9,PAF_Resource_ResolveRefWString
	STUB_END
	
	STUB_START "Kernel_Library",0x00090000,0x00020005
	STUB_FUNC  0xA089ECA4,sce_paf_private_memset
	STUB_FUNC  0x1839852A,sce_paf_private_memcpy
	STUB_END

	STUB_START "sceVshBridge",0x40090000,0x00010005
	STUB_FUNC 0x2380DC08, vshIoDevctl
	STUB_END

	//STUB_START "sceVshCommonGui",0x00090000,0x00010005
    //STUB_FUNC  0xD08C2C2B,sceVshCommonGuiDisplayContext
    //STUB_END

	//STUB_START "vsh",0x00090000,0x00020005
    //STUB_FUNC  0x3A370539,vshGetRegistryValue
    //STUB_FUNC  0xCD3AF2EC,vshSetRegistryValue
    //STUB_END
