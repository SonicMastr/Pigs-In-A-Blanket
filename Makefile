TARGET  = pib
OBJS    = main.o

LIBS    = -lSceShaccCg_stub_weak -ltaihen_stub -lSceLibc_stub -lSceLibKernel_stub -lSceKernelModulemgr_stub

PREFIX  ?= ${DOLCESDK}/arm-dolce-eabi
CC      = arm-dolce-eabi-gcc
AR      = arm-dolce-eabi-ar
CFLAGS  = -Wl,-q -Wall -O3 -nostartfiles -nostdlib
ASFLAGS = $(CFLAGS)

all: $(TARGET).suprx

%.suprx: %.velf
	dolce-make-fself $< $@

%.velf: %.elf
	dolce-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).suprx $(TARGET).velf $(TARGET).elf $(OBJS)