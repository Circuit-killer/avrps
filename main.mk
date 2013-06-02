TARGET ?= main
BASE ?= .

-include $(BASE)/config.mk

OPT = s  # GCC optimisations option
SRC ?= $(TARGET).c
CFLAGS =
CFLAGS += -g
CFLAGS += -O$(OPT) \
    -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
    -Wall -Wstrict-prototypes
CFLAGS += -std=gnu99
CFLAGS += -I$(BASE)
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRDUDE ?= avrdude
OBJ = $(SRC:.c=.o)
LST = $(SRC:.c=.lst)
DEPS = $(OBJ:.o=.d)
ALL_CFLAGS = -mmcu=$(MCU) $(CFLAGS)

all: $(TARGET).hex $(TARGET).lss

clean:
	rm -f $(TARGET).hex $(TARGET).elf $(OBJ) $(LST) $(DEPS) $(TARGET).map
	rm -f $(TARGET).lss

program: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

fuses: $(TARGET).elf
	$(OBJDUMP) -s -j .fuse $<
	@echo Program with:
	@echo $(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:0xNN:m

.PHONY: all clean program fuses

$(TARGET).hex: $(TARGET).elf
	@echo OBJCOPY $@
	$(OBJCOPY) --remove-section=.fuse -O ihex $< $@

$(TARGET).lss: $(TARGET).elf
	@echo OBJDUMP $@
	$(OBJDUMP) -h -S $< >$@

$(TARGET).elf: $(OBJ)
	@echo LD $@
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

$(LST): %.lst: %.o
LISTING = -Wa,-adhlns=$(<:.c=.lst)
AUTODEP = -MMD -MF $(@:.o=.d) -MP
$(OBJ): %.o: %.c
	@echo CC $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@ $(LISTING) $(AUTODEP)
-include $(DEPS)
