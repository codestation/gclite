TARGET = game_categories_light
OBJS = main.o logger.o category.o io.o scePaf.o clearcache.o gcpatches.o multims.o utils.o redirects.o

CFLAGS = -Os -G0 -Wall -fno-pic -fshort-wchar
# -DKPRINTF_ENABLED
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_USER_LIBS = 1
USE_USER_LIBC = 1

LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsystemctrl_user -lpspsysmem_user -lpsprtc

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak 
