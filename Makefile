TARGET = category_lite
STUBS = imports.o scePaf.o func_stubs.o
CATEGORY_MODES = multims.o context.o vshitem.o mode.o selection.o
HELPER = clearcache.o  logger.o utils.o config.o filter.o
OBJS = main.o category.o gcread.o gcpatches.o sysconf.o language.o
OBJS += $(CATEGORY_MODES) $(HELPER) $(STUBS) 
LIBS =  -lpsprtc -lpspreg

EXTRA_TARGETS = category_lang
EXTRA_CLEAN = category_lite_??.h

ifeq (x$(CONFIG_LANG), x)
CONFIG_LANG = en
endif

# use a psp-filer with VSH support or the plugin won't load on PRO firmware
# edit: 6.20 Pro won't work with this. Bugs, bugs everywhere....
all: category_lang
#	-psp-packer category_lite.prx

category_lang:
	bin2c lang/category_lite_$(CONFIG_LANG).txt category_lite_lang.h category_lite_lang

EXTRA_WARNS= -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wwrite-strings -Wunreachable-code
CFLAGS =-O2 -Wall -G0 -std=c99 -fshort-wchar $(EXTRA_WARNS)

ifeq ($(DEBUG), 1)
CFLAGS+=-DDEBUG
endif

ifeq ($(BENCHMARK), 1)
CFLAGS+=-DBENCHMARK
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
