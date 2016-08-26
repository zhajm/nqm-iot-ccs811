#include "ccs811.h"
#include "twi.h"

// I2C address
#define CCS_811_ADDRESS	0x5a
#define I2C_WRITE	((CCS_811_ADDRESS << 1) | 0)
#define I2C_READ	((CCS_811_ADDRESS << 1) | 1)

// I2C registers
#define STATUS_REG	0x00
#define MEAS_MODE_REG	0x01
#define ALG_RESULT_DATA	0x02
#define ENV_DATA	0x05
#define NTC_REG		0x06
#define THRESHOLDS	0x10
#define HW_ID_REG	0x20
#define ERR_ID_REG	0xE0
#define APP_START_REG	0xF4
#define SW_RESET	0xFF

#define GPIO_WAKE		0x5
#define INTERRUPT_DRIVEN	0x8
#define THRESHOLDS_ENABLED	0x4

// Measure mode register
#define DRIVE_MODE_IDLE		0x00
#define DRIVE_MODE_1SEC		0x10
#define DRIVE_MODE_10SEC	0x20
#define DRIVE_MODE_60SEC	0x30
#define INTERRUPT		0x08
#define THRESH			0x04

static uint8_t err;

#include <stdio.h>
void ccs811_init()
{
	// TODO: check for status
	//ccs811_read(STATUS_REG);
}

void ccs811_test()
{
	printf("\n\e[1;36m%s\e[0m\n", __func__);
	printf("CCS811 status: %02x, (%02x)\n", ccs811_read_status(), err);
	printf("CCS811 mode: %02x (%02x)\n", ccs811_read_mode(), err);
	//printf("CCS811 poll: %02x (%02x)\n", ccs811_poll(), err);
	if (ccs811_poll()) {
		static struct ccs811_data_t data;
		ccs811_read_data(&data);
		printf("CCS811 data (%02x)\n", err);
	}
}

uint8_t ccs811_error()
{
	return err;
}

uint8_t ccs811_read(uint8_t addr)
{
	err = 0;
	twi_start();
	err += !twi_write(I2C_WRITE);
	err += !twi_write(addr);

	// Repeated start
	twi_start();
	err += !twi_write(I2C_READ);
	if (err) {
		twi_stop();
		return 0;
	}
	uint8_t data = twi_read(1);
	twi_stop();
	return err ? 0 : data;
}

uint8_t ccs811_read_multi(uint8_t addr, uint8_t len, uint8_t *ptr)
{
	err = 0;
	twi_start();
	err += !twi_write(I2C_WRITE);
	err += !twi_write(addr);

	// Repeated start
	twi_start();
	err += !twi_write(I2C_READ);
	if (err) {
		twi_stop();
		return err;
	}

	while (len--)
		*ptr++ = twi_read(len);
	twi_stop();
	return err;
}

uint8_t ccs811_read_status()
{
	return ccs811_read(STATUS_REG);
}

uint8_t ccs811_read_mode()
{
	return ccs811_read(MEAS_MODE_REG);
}

uint8_t ccs811_poll()
{
	return ccs811_read_status() & CCS811_DATA_READY;
}

static inline uint16_t swap_u16(uint16_t data)
{
	return ((data & 0xff) << 8) | ((data >> 8) & 0xff);
}

void ccs811_read_data(struct ccs811_data_t *ptr)
{
	ccs811_read_multi(ALG_RESULT_DATA, sizeof(struct ccs811_data_t), (void *)ptr);
	ptr->eco2 = swap_u16(ptr->eco2);
	ptr->tvoc = swap_u16(ptr->tvoc);
	ptr->raw = swap_u16(ptr->raw);
}
