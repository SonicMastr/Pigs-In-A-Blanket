USE_VITASDK ?= 0

ifeq ($(USE_VITASDK),1)
	SDKPATH = $(VITASDK)
	SDKPREFIX = vita
	CFLAGS = -DUSE_VITASDK
else
	SDKPATH = $(DOLCESDK)
	SDKPREFIX = dolce
	CFLAGS = -DUSE_DOLCESDK
endif

LIB     = pib
OBJS    = src/pib.o src/hooks.o src/shacccgpatch.o src/patches.o src/sha1.o src/sysmodepatch.o src/essl.o
INCLUDE = include

PREFIX ?= $(SDKPATH)/arm-$(SDKPREFIX)-eabi
CC      = arm-$(SDKPREFIX)-eabi-gcc
AR      = arm-$(SDKPREFIX)-eabi-ar
CFLAGS += -Wl,-q -Wall -Wno-incompatible-pointer-types -Wno-pointer-sign -O3 -nostartfiles -nostdlib -DVITA -D__VITA__
ASFLAGS = $(CFLAGS)

all: lib

debug: CFLAGS += -DDEBUG
debug: lib

lib: lib$(LIB).a

%.a: $(OBJS) 
	$(AR) -rc $@ $^

clean:
	@rm -rf combine $(OBJS) lib$(LIB).a

install: all
	@mkdir -p $(DESTDIR)$(PREFIX)/include/
	@cp include/pib.h $(DESTDIR)$(PREFIX)/include/
	@cp -r include/EGL $(DESTDIR)$(PREFIX)/include/
	@cp -r include/GLES2 $(DESTDIR)$(PREFIX)/include/
	@cp -r include/KHR $(DESTDIR)$(PREFIX)/include/
	@mkdir -p $(DESTDIR)$(PREFIX)/lib/
	@cp lib$(LIB).a $(DESTDIR)$(PREFIX)/lib/
	@echo Installed PIB!

.PHONY: install clean
