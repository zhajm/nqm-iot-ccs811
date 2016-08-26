#ifndef CCS811_H
#define CCS811_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Status register
#define CCS811_FW_MODE		0x80
#define CCS811_APP_VALID	0x10
#define CCS811_DATA_READY	0x08
#define CCS811_ERR		0x01

void ccs811_init();

struct ccs811_data_t {
	uint16_t eco2;
	uint16_t tvoc;
	uint8_t errorID;
	uint8_t status;
	uint16_t raw;
};

uint8_t ccs811_error();
uint8_t ccs811_read_status();
uint8_t ccs811_read_mode();
uint8_t ccs811_poll();
void ccs811_read_data(struct ccs811_data_t *ptr);

#ifdef __cplusplus
}
#endif

#endif // CCS811_h
