#include <avr/io.h>
#include "twi.h"
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
	twi_read_data(I2C_ADDRESS, REG_RH_HOLD, 2, data);
	res.rh = ((uint16_t)data[0] << 8) | ((uint16_t)data[1]);
	twi_read_data(I2C_ADDRESS, REG_TEMP_PREV, 2, data);
	res.temp = ((uint16_t)data[0] << 8) | ((uint16_t)data[1]);
	return res;
}

uint8_t si7021_read_user1()
{
	uint8_t data;
	if (!twi_read_data(I2C_ADDRESS, REG_USER1_R, 1, &data))
		return 0;
	return data;
}

uint16_t si7021_read_temp()
{
	uint8_t data[2];
	twi_read_data(I2C_ADDRESS, REG_TEMP_HOLD, 2, data);
	return ((uint16_t)data[0] << 8) | ((uint16_t)data[1]);
}

uint32_t si7021_read_u16_u32(uint16_t reg)
{
	uint32_t data = 0;
	uint8_t err = 0;
	twi_start();
	err += !twi_write(I2C_ADDRESS << 1);
	err += !twi_write(reg >> 8);
	err += !twi_write(reg & 0xff);
	twi_start();
	err += !twi_write((I2C_ADDRESS << 1) | 1U);
	if (err)
		return 0;
	uint8_t i = 6;
	while (i--) {
		if ((i % 3) == 0) {
			twi_read(i);	// Skip CRC
			continue;
		}
		data <<= 8;
		data |= (uint32_t)twi_read(i);
	}
	twi_stop();
	return data;
}

uint64_t si7021_read_sn()
{
	uint64_t data = (uint64_t)si7021_read_u16_u32(REG_SN_1) << 32;
	data |= si7021_read_u16_u32(REG_SN_2);
	return data;
}

uint8_t si7021_write_heater(uint8_t heater)
{
	uint8_t data;
	if (!twi_read_data(I2C_ADDRESS, REG_USER1_R, 1, &data))
		return 0;
	if (heater & 0x80)
		data |= REG_USER1_HTRE;
	else
		data &= ~REG_USER1_HTRE;
	if (!twi_write_data(I2C_ADDRESS, REG_USER1_W, 1, &data))
		return 0;
	if (!twi_read_data(I2C_ADDRESS, REG_HEATER_R, 1, &data))
		return 0;
	data = (data & 0xf0) | (heater & 0x0f);
	return twi_write_data(I2C_ADDRESS, REG_HEATER_W, 1, &data);
}
