MCU = attiny2313
TARGET = main

-include config.mk

OPT = s
FORMAT = ihex
SRC = $(TARGET).c timer.c
CFLAGS =
CFLAGS += -g
CFLAGS += -O$(OPT) \
    -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
    -Wall -Wstrict-prototypes \
    -Wa,-adhlns=$(<:.c=.lst)
CFLAGS += -std=gnu99
CFLAGS += -I../../include
#CFLAGS += -mrelax # -8/2086 bytes code
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRDUDE = avrdude
OBJ = $(SRC:.c=.o)
LST = $(SRC:.c=.lst)
ALL_CFLAGS = -mmcu=$(MCU) $(CFLAGS)

all: $(TARGET).hex $(TARGET).lss

clean:
	rm -f $(TARGET).hex $(TARGET).elf $(OBJ) $(LST) $(TARGET).map
	rm -f $(TARGET).lss

program: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

.PHONY: all clean program

$(TARGET).hex: $(TARGET).elf
	@echo OBJCOPY $@
	$(OBJCOPY) -O $(FORMAT) $< $@

$(TARGET).lss: $(TARGET).elf
	@echo OBJDUMP $@
	$(OBJDUMP) -h -S $< >$@

$(TARGET).elf: $(OBJ)
	@echo LD $@
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

$(OBJ): %.o: %.c
	@echo CC $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@
