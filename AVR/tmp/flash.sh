#!/bin/bash
tty=/dev/ttyUSB0
baud=38400

echo "****** AVRDUDE ******"
echo "avrdude -b 57600 -c arduino -p atmega328p -P $tty -U flash:w:$@"
avrdude -b 57600 -c arduino -p atmega328p -P $tty -U flash:w:$@

sleep 1

echo "****** MICROCOM ******"
echo "microcom -p $tty -s $baud"
microcom -p $tty -s $baud
