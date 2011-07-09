    .set noreorder

#include "pspstub.s"

    STUB_START "SystemCtrlForUser",0x00090000,0x00020005
    STUB_FUNC 0x1C90BECB,sctrlHENSetStartModuleHandler
    STUB_FUNC 0x159AF5CC,sctrlHENFindFunction
    STUB_END
