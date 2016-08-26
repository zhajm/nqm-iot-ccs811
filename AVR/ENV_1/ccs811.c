#include <avr/pgmspace.h>
#include <util/delay.h>
#include "ccs811.h"
#include "twi.h"

#define info(str)	puts_P(PSTR(str))

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
#define HW_VERSION	0x21
#define FW_VERSION	0x24
#define ERR_ID_REG	0xE0
#define APP_START_REG	0xF4
#define SW_RESET	0xFF
#define APP_ERASE	0xf1
#define APP_DATA	0xf2
#define APP_VERIFY	0xf3
#define APP_START	0xf4

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

static inline uint16_t swap_u16(uint16_t data)
{
	return ((data & 0xff) << 8) | ((data >> 8) & 0xff);
}

#include <stdio.h>
uint8_t ccs811_init()
{
#if 0
	// Reset the sensor
	static const uint8_t seq[] = {0x11, 0xe5, 0x72, 0x8a};
	if (!twi_write_data(CCS811_ADDRESS, SW_RESET, 4, seq)) {
		info("Error reseting sensor");
		return 0;
	}
	_delay_ms(100);
#endif

#if 1
	// Start application
	while (!(ccs811_read_status() & CCS811_FW_MODE)) {
		info("Starting sensor...");
		if (!twi_write_data(CCS811_ADDRESS, APP_START_REG, 0, 0))
			info("Error starting sensor");
		//_delay_ms(100);
	}
#else
	// Start application
	if (!(ccs811_read_status() & CCS811_FW_MODE))
		info("Starting sensor...");
	if (!(ccs811_read_status() & CCS811_FW_MODE) &&
			!twi_write_data(CCS811_ADDRESS, APP_START_REG, 0, 0))
		info("Error starting sensor");
#endif

	// Check status
	static const uint8_t check = CCS811_FW_MODE | CCS811_APP_VALID;
	if ((ccs811_read_status() & check) != check) {
		info("Sensor status error");
		return 0;
	}

	// Set measure mode
	static const uint8_t mode = 0x10;
	if (!twi_write_data(CCS811_ADDRESS, MEAS_MODE_REG, 1, &mode)) {
		info("Set measure mode failed");
		return 0;
	}
	return 1;
}

uint8_t ccs811_reset()
{
	// Reset the sensor
	const uint8_t seq[] = {0x11, 0xe5, 0x72, 0x8a};
	return twi_write_data(CCS811_ADDRESS, SW_RESET, 4, seq);
}

uint8_t ccs811_erase()
{
	// Erase the sensor application code
	const uint8_t seq[] = {0xe7, 0xa7, 0xe6, 0x09};
	return twi_write_data(CCS811_ADDRESS, APP_ERASE, 4, seq);
}

uint8_t ccs811_program_P(PGM_VOID_P ptr, uint8_t size)
{
	uint8_t seq[8], *sptr = seq, i = sizeof(seq);
	while (i--)
		*sptr++ = pgm_read_byte(ptr++);
	return twi_write_data(CCS811_ADDRESS, APP_DATA, sizeof(seq), seq);
}

uint8_t ccs811_verify()
{
	return twi_write_data(CCS811_ADDRESS, APP_VERIFY, 0, 0);
}

uint16_t ccs811_read_fw_version()
{
	uint16_t ver;
	twi_read_data(CCS811_ADDRESS, FW_VERSION, 2, (void *)&ver);
	return swap_u16(ver);
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
	//static uint8_t size = 4;
	uint8_t *ptr = (uint8_t *)&data;
	printf("| ENVW:\t");
	printf("0x%02x%02x\t", ptr[0], ptr[1]);
	printf("0x%02x%02x\t", ptr[2], ptr[3]);
	//if (size == 9)
	//	size = 2;
	return twi_write_data(CCS811_ADDRESS, ENV_DATA, 4, (void *)&data);
}

#include "escape.h"
void ccs811_test()
{
#if 1
	printf("| ERR:\t0x%02x\n", ccs811_read_error());
#if 0
	uint8_t data[4];
	twi_read_data(CCS811_ADDRESS, ENV_DATA, 4, data);
	printf("| ENVR:\t");
	printf("0x%02x%02x\t", data[0], data[1]);
	printf("0x%02x%02x\n", data[2], data[3]);
#endif
#endif
#if 0
	static uint8_t i = 0;
	printf("\n\n" ESC_CYAN "*** TEST %u ***\n", i++);
	uint8_t x, y;
	for (x = 0; x != 8; x++)
		printf("%8u ", x);
	putchar('\n');
	uint8_t reg = 0;
	for (y = 0; y != 0x20; y++) {
		for (x = 0; x != 8; x++) {
			const static uint8_t wr[4] = {0x1f, 0xaa, 0x64, 0x55};
			twi_write_data(CCS811_ADDRESS, reg, sizeof(wr), wr);
			uint8_t data[4];
			if (twi_read_data(CCS811_ADDRESS, reg++, sizeof(data), data)) {
				if (ccs811_read_error())
					printf(ESC_YELLOW);
				else
					printf(ESC_GREEN);
			} else
				printf(ESC_RED);
			if (*((uint32_t *)data) == 0xff000400)
				printf(ESC_GREY);
			uint8_t i;
			for (i = 0; i != sizeof(data); i++)
				printf("%02x", data[i]);
			putchar(' ');
		}
		putchar('\n');
	}
#endif
}
