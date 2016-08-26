#include <util/delay.h>
#include "ccs811.h"
#include "twi.h"

// I2C address
#define CCS811_ADDRESS	0x5a
#define I2C_WRITE	((CCS811_ADDRESS << 1) | 0)
#define I2C_READ	((CCS811_ADDRESS << 1) | 1)

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

#define FLOAT2U16(v)	((uint16_t)((float)(v) * (float)(1U << 9)))

#include <stdio.h>
uint8_t ccs811_init()
{
#if 0
	uint8_t data[4];
	// Reset the sensor
	static const uint8_t seq[] = {0x11, 0xe5, 0x72, 0x8a};
	if (!twi_write_data(CCS811_ADDRESS, SW_RESET, 4, seq)) {
		puts("Error reseting sensor");
		return 0;
	}
	twi_read_data(CCS811_ADDRESS, 0xFF, 4, data);
	printf("SW_RESET: 0x%02x 0x%02x 0x%02x 0x%02x\n", data[0], data[1], data[2], data[3]);
	_delay_ms(3);
#endif

	// Start application
	if (!(ccs811_read_status() & CCS811_FW_MODE) &&
			!twi_write_data(CCS811_ADDRESS, APP_START_REG, 0, 0)) {
		puts("Error start sensor");
		return 0;
	}

	// Check status
	static const uint8_t check = CCS811_FW_MODE | CCS811_APP_VALID;
	if ((ccs811_read_status() & check) != check) {
		puts("Sensor status error");
		return 0;
	}

	// Set measure mode
	static const uint8_t mode = 0x10;
	if (!twi_write_data(CCS811_ADDRESS, MEAS_MODE_REG, 1, &mode)) {
		puts("Set measure mode failed");
		return 0;
	}
	return 1;
}

static inline uint8_t ccs811_read(uint8_t addr)
{
	uint8_t data;
	return twi_read_data(CCS811_ADDRESS, addr, 1, &data) ? data : 0;
}

uint8_t ccs811_read_status()
{
	return ccs811_read(STATUS_REG);
}

uint8_t ccs811_read_error()
{
	return ccs811_read(ERR_ID_REG);
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
	twi_read_data(CCS811_ADDRESS, ALG_RESULT_DATA, sizeof(struct ccs811_data_t), (void *)ptr);
	ptr->eco2 = swap_u16(ptr->eco2);
	ptr->tvoc = swap_u16(ptr->tvoc);
	ptr->raw = swap_u16(ptr->raw);
}

uint8_t ccs811_write_envdata(float rh, float temp)
{
	struct {
		uint16_t rh;
		uint16_t temp;
	} data = {
		swap_u16(FLOAT2U16(rh)),
		swap_u16(FLOAT2U16(temp + 25.)),
	};
	uint8_t *ptr = (uint8_t *)&data;
	printf("| ENVW:\t");
	printf("0x%02x%02x\t", ptr[0], ptr[1]);
	printf("0x%02x%02x\t", ptr[2], ptr[3]);
	return twi_write_data(CCS811_ADDRESS, ENV_DATA, 4, (void *)&data);
}

void ccs811_test()
{
	uint8_t data[4] = {0};
	twi_read_data(CCS811_ADDRESS, ENV_DATA, 4, data);
	printf("| ENVR:\t");
	printf("0x%02x%02x\t", data[0], data[1]);
	printf("0x%02x%02x\n", data[2], data[3]);
}
