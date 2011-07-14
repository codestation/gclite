TARGET = category_lite
STUBS = imports.o scePaf.o func_stubs.o
CATEGORY_MODES = multims.o context.o vshitem.o
HELPER = clearcache.o  logger.o utils.o config.o
OBJS = main.o category.o gcread.o gcpatches.o sysconf.o language.o
OBJS += $(CATEGORY_MODES) $(HELPER) $(STUBS) 
LIBS =  -lpsprtc -lpspreg

EXTRA_TARGETS = category_lang
EXTRA_CLEAN = category_lite_??.h

ifeq (x$(CONFIG_LANG), x)
CONFIG_LANG = en
endif

all: category_lang
	-pspgz.py category_lite.prx Game_Categories_Light 0x0007

category_lang:
	bin2c category_lite_$(CONFIG_LANG).txt category_lite_lang.h category_lite_lang

CFLAGS = -O2 -G0 -Wall -std=c99 -fshort-wchar

ifeq ($(DEBUG), 1)
CFLAGS+=-DDEBUG
endif

ASFLAGS = $(CFLAGS)
LDFLAGS = -mno-crt0 -nostartfiles

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_USER_LIBS = 1
USE_USER_LIBC = 1

PSP_FW_VERSION=620

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
