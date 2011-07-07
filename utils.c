#include "game_categories_light.h"
#include "logger.h"

void *redir2stub(u32 address, u32 stub, void *redir) {
    _sw(_lw(address), stub);
    _sw(_lw(address + 4), stub + 4);
    MAKE_JUMP(stub + 8, address + 8);
    MAKE_STUB(address, redir);
    return (void *)stub;
}

void *redir_call(u32 address, void *func) {
    void *f = (void *)U_EXTRACT_CALL(address);
    MAKE_CALL(address, func);
    return f;
}
