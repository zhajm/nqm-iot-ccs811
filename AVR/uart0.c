#include <avr/io.h>
#include "uart0.h"

void uart0_init()
{
        // GPIO initialisation
        //DDRD &= ~_BV(0);		//input
	//DDRD |=	_BV(1);			//output
        //PORTD |= _BV(0) | _BV(1);	//pull-up

        // UART 0 initialisation
        #include <util/setbaud.h>
        UBRR0H = UBRRH_VALUE;
        UBRR0L = UBRRL_VALUE;
        UCSR0A = USE_2X << U2X0;
        UCSR0B = _BV(RXEN0) | _BV(TXEN0);
        UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
	
	//STDOUT
	stdout = uart0_fd();
}

char uart0_read()
{
	while (!(UCSR0A & _BV(RXC0)));	//RXC0 goes high when data received
	return UDR0;			//Read from buffer
}

void uart0_write(const char c)
{
	while (!(UCSR0A & _BV(UDRE0)));	//Check buffer status,write when 0
	UDR0 = c;			//Write c to buffer
}

void uart0_write_data(const char *ptr, unsigned long length)
{
		while (length--)
		uart0_write(*ptr++);
}

// For fdevopen
static int putch(char ch, FILE *stream)
{
        if (ch == '\n')
                putch('\r', stream);
        uart0_write(ch);
        return ch;
}

// For fdevopen
static int getch(FILE *stream)
{
        return uart0_read();
}

FILE *uart0_fd()
{
        static FILE *dev = NULL;
        if (dev == NULL)
                dev = fdevopen(putch, getch);
        return dev;
}
