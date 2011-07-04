TARGET = category_lite
STUBS = imports.o scePaf.o
OBJS = main.o category.o io.o gcread.o clearcache.o gcpatches.o multims.o redirects.o sysconf.o logger.o vshitem.o
OBJS += $(STUBS)
LIBS =  -lpsprtc

CFLAGS = -O2 -G0 -Wall -std=c99 -fno-pic -fshort-wchar -DDEBUG
ASFLAGS = $(CFLAGS)

PRX_EXPORTS = exports.exp

USE_USER_LIBS = 1
USE_USER_LIBC = 1

PSP_FW_VERSION=620

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
