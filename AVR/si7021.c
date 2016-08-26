#include <avr/io.h>
#include "i2c.h"
#include "si7021.h"

#define I2C_ADDRESS	0x40
#define REG_RH_HOLD	0xe5
#define REG_RH		0xf5
#define REG_TEMP_HOLD	0xe3
#define REG_TEMP	0xf3
#define REG_TEMP_PREV	0xe0
#define REG_RESET	0xfe

#define REG_USER1_W	0xe6
#define REG_USER1_R	0xe7

#define REG_HEATER_W	0x51
#define REG_HEATER_R	0x11

#define REG_SN_1	0xfa0f
#define REG_SN_2	0xfcc9
#define REG_REV		0x84b8

struct si7021_data_t si7021_read()
{
	struct si7021_data_t res;
	uint8_t data[2];
	i2c_read_data(I2C_ADDRESS, REG_RH_HOLD, 2, data);
	res.rh = ((uint16_t)data[0] << 8) | ((uint16_t)data[1]);
	i2c_read_data(I2C_ADDRESS, REG_TEMP_PREV, 2, data);
	res.temp = ((uint16_t)data[0] << 8) | ((uint16_t)data[1]);
	return res;
}

#if 0
#include <avr/io.h>
#include "i2c.h"
#include "si7021.h"
#include "ccs811.h"

#define SI_7021_ADDRESS 0x40
#define SI_I2C_WRITE	((SI_811_ADDRESS << 1) | 0)
#define SI_I2C_READ	((SI_811_ADDRESS << 1) | 1)

#define MEASURE_RH_HOLDMASTER	0xE5
#define MEASURE_T_HOLDMASTER	0xE3

#define RESET	0xFE

static uint8_t error;

uint16_t humidity;
uint16_t temperature;

uint8_t si7021_read(uint8_t command uint8_t len, uint8_t *ptr)
{
	error = 0;
	i2c_start();
	error += !i2c_write(SI_I2C_WRITE);
	error += !i2c_write(command);

	i2c_start();
	error += !i2c_write(SI_I2C_READ);
	if (error) {
		i2c_stop();
		return error;
	}

	while (len--)
		*ptr++ = i2c_read(len);
	i2c_stop();
	return error;
}

uint8_t si7021_rh_polling(struct si7021_data_t *ptr)
{
	si7021_read(MEASURE_RH_HOLDMASTER, 2, (void *)ptr);
	si7021_read(MEASURE_T_HOLDMASTER, 2, (void *)ptr);
}
#endif
