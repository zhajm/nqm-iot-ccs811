#ifndef CCS811_H
#define CCS811_H

#include <stdint.h>
#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

// Status register
#define CCS811_FW_MODE		0x80
#define CCS811_APP_VALID	0x10
#define CCS811_DATA_READY	0x08
#define CCS811_ERR		0x01
// Only in bootloader mode
#define CCS811_ERSE_READY	0x40
#define CCS811_VERIFY_READY	0x20

uint8_t ccs811_init();

//uint8_t ccs811_reset();
//uint8_t ccs811_erase();
//uint8_t ccs811_program_P(PGM_VOID_P ptr, uint8_t size);
//uint8_t ccs811_verify();
//uint16_t ccs811_read_fw_version();

struct ccs811_data_t {
	uint16_t eco2;
	uint16_t tvoc;
	uint8_t errorID;
	uint8_t status;
	uint16_t raw;
};

//uint8_t ccs811_read_status();
//uint8_t ccs811_read_error();
uint16_t swap_u16(uint16_t data);
uint8_t ccs811_read(uint8_t addr);
uint8_t ccs811_poll();
void ccs811_read_data(struct ccs811_data_t *ptr);
uint8_t ccs811_write_envdata(float rh, float temp);
//void ccs811_test();

#ifdef __cplusplus
}
#endif

#endif // CCS811_h


#if 0
#ifndef CCS811_H
#define CCS811_H

#include <stdio.h>

#ifdef _cplusplus
extern "C" {
#endif

extern uint8_t data;
extern uint64_t result;

uint8_t ccs811_init();

struct ccs811_data_t {
	uint16_t eco2;
	uint16_t tvoc;
	uint8_t errorID;
	uint8_t status;
	uint16_t raw;
};

struct si7021_data_t {
	uint16_t humidity;
	uint16_t temperature;
};

struct calibration {
	uint16_t rh;
	uint16_t temp;
};

struct env {
	uint16_t rh1;
	uint16_t t1;
};

uint8_t ccs811_write(uint8_t address, uint8_t instructions);
uint8_t ccs811_read(uint8_t addr);
uint8_t device_read_multi(uint8_t device_w, uint8_t device_r, uint8_t addr, uint8_t len, uint8_t *ptr);
uint8_t device_write_multi(uint8_t device_w, uint8_t addr, uint8_t len, uint8_t *ptr);
uint8_t ccs811_polling(struct ccs811_data_t *ptr);
uint8_t si7021_polling(struct si7021_data_t *pointer);
uint8_t error();
uint8_t ccs811_poll();
uint8_t ccs811_calib(float humidity, float temperature);

#ifdef _cplusplus
}
#endif

#endif
#endif


