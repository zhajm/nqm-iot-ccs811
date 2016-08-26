# Modified by zhiyb (Yubo Zhi)

TRG	= tmp
SRC	= main.c uart0.c twi.c ccs811.c si7021.c am2302.c
SUBDIRS	=
INCDIRS	= .

BAUD		= 38400
TTY		= /dev/ttyUSB0
PRGER		= arduino -b 57600 -p atmega328p -P $(TTY)
#MCU_TARGET	= atmega644p
#MCU_FREQ	= 12000000UL
MCU_TARGET	= atmega328p
MCU_FREQ	= 8000000UL

DEFS	+= -DUART0_BAUD=38400
LIBS	+= -Wl,-u,vfprintf -lprintf_flt -lm

SRCDIR	?= .
include $(SRCDIR)/Makefile_AVR.defs

CLEAN_FILES	+= $(TRG).hex.up flash.sh.up
.PHONY: upload
upload: $(TRG).hex.up flash.sh.up

%.up: %
	scp -prBC -P 6022 $^ junming@localhost:remote/
	touch $@

.PHONY: remote
remote: upload
	ssh -p 6022 junming@localhost "cd remote; chmod a+x flash.sh; ./flash.sh $(TRG).hex"

CLEAN_FILES	+= archive.tar.bz2
archive.tar.bz2: $(TRG).hex $(wildcard Makefile* *c *.h)
	tar jcvf archive.tar.bz2 *.c *.h Makefile*

CLEAN_FILES	+= archive.tar.bz2.up
.PHONY: upload-archive
upload-archive: archive.tar.bz2.up
	ssh -p 6022 junming@localhost "rm -rf remote/code; mkdir -p remote/code; cd remote/code; tar jxvf ../archive.tar.bz2"

.PHONY: com
com:
	microcom -s $(BAUD) -p $(TTY)
