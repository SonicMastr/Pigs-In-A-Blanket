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

# vitasdk does not include a piglet stub, so generate that automatically
ifeq ($(USE_VITASDK),1)
all: stub
debug: stub
endif

lib: lib$(LIB).a

stub: piglet_stub/out/liblibScePiglet_stub.a shacc_stub/out/libSceShaccCg_stub.a

piglet_stub/out/liblibScePiglet_stub.a:
	$(MAKE) -C piglet_stub SDKPREFIX=$(SDKPREFIX)

shacc_stub/out/libSceShaccCg_stub.a:
	$(MAKE) -C shacc_stub SDKPREFIX=$(SDKPREFIX)

%.a: $(OBJS) 
	$(AR) -rc $@ $^

clean:
	@$(MAKE) -C piglet_stub clean
	@$(MAKE) -C shacc_stub clean
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

# when using vitasdk, also install the stub when installing libpib
ifeq ($(USE_VITASDK),1)
install: install_stub
endif

install_stub: piglet_stub/out/liblibScePiglet_stub.a shacc_stub/out/libSceShaccCg_stub.a
	@mkdir -p $(DESTDIR)$(PREFIX)/lib/
	@cp piglet_stub/out/liblibScePiglet_stub.a $(DESTDIR)$(PREFIX)/lib/
	@cp shacc_stub/out/libSceShaccCg_stub.a $(DESTDIR)$(PREFIX)/lib/

.PHONY: install install_stub clean
