LIB  	= pib
OBJS    = src/main.o src/hooks.o src/shacccgpatch.o src/patches.o
INCLUDE = include

PREFIX  ?= ${DOLCESDK}/arm-dolce-eabi
CC      = arm-dolce-eabi-gcc
AR      = arm-dolce-eabi-ar
CFLAGS  = -Wl,-q -Wall -Wno-incompatible-pointer-types -Wno-pointer-sign -O3 -nostartfiles -nostdlib -D__VITA__
ASFLAGS = $(CFLAGS)

all: lib

debug: CFLAGS += -DDEBUG_MODE
debug: lib

lib: lib$(LIB).a

%.a: $(OBJS) 
	$(AR) -rc $@ $^

clean:
	@rm -rf $(OBJS) lib$(LIB).a
	
install: lib$(LIB).a
	@mkdir -p $(DESTDIR)$(PREFIX)/lib/
	cp lib$(LIB).a $(DESTDIR)$(PREFIX)/lib/
	@mkdir -p $(DESTDIR)$(PREFIX)/include/
	cp include/pib.h $(DESTDIR)$(PREFIX)/include/
	cp -r include/EGL $(DESTDIR)$(PREFIX)/include/
	cp -r include/GLES2 $(DESTDIR)$(PREFIX)/include/
	cp -r include/KHR $(DESTDIR)$(PREFIX)/include/