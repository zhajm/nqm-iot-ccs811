SRC = main.c ccs811.c i2c.c uart0.c si7021.c
TRG = AVR

MCU = atmega328p
CPU = 8000000UL
TTY = /dev/ttyUSB0
BAUD = 38400
AVRDUDE = avrdude -b 57600 -c arduino -P $(TTY)

OBJ = $(SRC:.c=.o)

override CFLAGS = -Os -Os -mmcu=$(MCU) -DF_CPU=$(CPU) -DBAUD=$(BAUD) -Wall -Wextra -Wno-unused-parameter
override LDFLAGS = -mmcu=$(MCU) -Wl,-u,vfprintf -lprintf_flt -lm

.PHONY: all
all: $(TRG).hex

# Source file dependency
-include $(OBJ:%.o=%.d)

.PHONY: com
com:
	microcom -p $(TTY) -s $(BAUD)

.PHONY: flash
flash: $(TRG).hex
	$(AVRDUDE) -p $(MCU) -U flash:w:$<

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

$(TRG).elf: $(OBJ)
	avr-gcc $(LDFLAGS) -o $@ $^

%.o: %.c
	avr-gcc $(CFLAGS) -c -o $@ $< -MD -MP -MF $*.d -Werror

clean:
	-rm -f $(OBJ) $(OBJ:.o=.d) $(TRG).elf $(TRG).hex
