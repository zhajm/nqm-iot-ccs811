#ifndef I2C_H
#define I2C_H

#include <stdio.h>

#ifdef _cplusplus
extern "C" {
#endif

void i2c_init();
void i2c_start();
uint8_t i2c_write(unsigned char data);
uint8_t i2c_read(uint8_t ack);
void i2c_stop();
uint8_t i2c_write_data(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *ptr);
uint8_t i2c_read_data(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *ptr);

#ifdef _cplusplus
}
#endif

#endif
