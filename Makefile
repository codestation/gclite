TARGET = category_lite
OBJS = main.o logger.o category.o io.o gcread.o scePaf.o clearcache.o gcpatches.o multims.o redirects.o
LIBS = -lpspsystemctrl_user -lpspsysmem_user -lpsprtc -lpspkubridge

CFLAGS = -O2 -G0 -Wall -std=c99 -fno-pic -fshort-wchar
# -DKPRINTF_ENABLED
ASFLAGS = $(CFLAGS)

PRX_EXPORTS = exports.exp

USE_USER_LIBS = 1
USE_USER_LIBC = 1

PSP_FW_VERSION=620

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
