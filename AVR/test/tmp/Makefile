# Modified by zhiyb (Yubo Zhi)

TRG	= tmp
SRC	= main.c uart0.c twi.c ccs811.c
SUBDIRS	=
INCDIRS	= .

PRGER		= usbasp
#MCU_TARGET	= atmega644p
#MCU_FREQ	= 12000000UL
MCU_TARGET	= atmega328p
MCU_FREQ	= 8000000UL

DEFS	+= -DUART0_BAUD=38400
LIBS	+= -lm

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs

.PHONY: upload
upload: $(TRG).hex.up flash.sh.up

%.up: %
	scp -prBC -P 6022 $^ junming@localhost:remote/
	touch $@

.PHONY: remote
remote: upload
	ssh -p 6022 junming@localhost "cd remote; chmod a+x flash.sh; ./flash.sh $(TRG).hex"
