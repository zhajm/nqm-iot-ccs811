#ifndef UART_H
#define UART_H

#include <stdio.h>

#ifdef _cplusplus
extern "C" {
#endif

void uart0_init();
char uart0_read();
void uart0_write(const char c);
void uart0_write_data(const char *ptr, unsigned long length);
FILE *uart0_fd();

#ifdef _cplusplus
}
#endif

#endif
