# Modified by zhiyb (Yubo Zhi)

TRG	= tmp
SRC	= main.c uart0.c twi.c
SUBDIRS	=
INCDIRS	= .

TTY		= /dev/ttyUSB0
BAUD		= 9600
PRGER		= arduino -b 57600 -P $(TTY)
#MCU_TARGET	= atmega644p
#MCU_FREQ	= 12000000UL
MCU_TARGET	= atmega328p
MCU_FREQ	= 8000000UL

DEFS	+= -DUART0_BAUD=$(BAUD)
LIBS	+= -lm

SRCDIR	?= .
include $(SRCDIR)/Makefile_AVR.defs

.PHONY: com
com:
	microcom -p $(TTY) -s $(BAUD)

.PHONY: upload
upload: $(TRG).hex.up flash.sh.up

%.up: %
	scp -prBC -P 6022 $^ junming@localhost:remote/
	touch $@

.PHONY: remote
remote: upload
	ssh -p 6022 junming@localhost "cd remote; chmod a+x flash.sh; ./flash.sh $(TRG).hex"
