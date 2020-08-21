PLUGIN  = pib
LIB 	= pib.a
OBJS    = src/main.o

LIBS    = -lSceShaccCg_stub_weak -ltaihen_stub -lSceLibc_stub -lSceLibKernel_stub -lSceKernelModulemgr_stub

PREFIX  ?= ${DOLCESDK}/arm-dolce-eabi
CC      = arm-dolce-eabi-gcc
AR      = arm-dolce-eabi-ar
CFLAGS  = -Wl,-q -Wall -Wincompatible-pointer-types -Wpointer-sign -O3 -nostartfiles -nostdlib
ASFLAGS = $(CFLAGS)

all: plugin lib

debug: CFLAGS += -DDEBUG_MODE
debug: all

plugin: $(PLUGIN).suprx

lib: CFLAGS += -DLIB
lib: $(LIB)

%.suprx: %.velf
	dolce-make-fself $< $@

%.velf: %.elf
	dolce-elf-create $< $@

$(PLUGIN).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(LIB): $(OBJS) 
	$(AR) -rc $@ $^

clean:
	@rm -rf $(PLUGIN).suprx $(PLUGIN).velf $(PLUGIN).elf $(OBJS) $(LIB)

install: $(LIB)
	@mkdir -p $(DESTDIR)$(PREFIX)/lib/
	cp $(LIB) $(DESTDIR)$(PREFIX)/lib/
	@mkdir -p $(DESTDIR)$(PREFIX)/include/
	cp include/pib.h $(DESTDIR)$(PREFIX)/include/