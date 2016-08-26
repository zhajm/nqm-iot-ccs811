#ifndef TWI_H
#define TWI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void twi_init();
void twi_start();
void twi_stop();
uint8_t twi_write(uint8_t data);
uint8_t twi_read(uint8_t ack);
uint8_t twi_scan(uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif // TWI_H
