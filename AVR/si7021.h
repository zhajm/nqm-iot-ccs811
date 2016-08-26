#ifndef SI7021_H
#define SI7021_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// USER1 register bitfields
#define REG_USER1_HTRE	(1U << 2)

struct si7021_data_t
{
	uint16_t rh;
	uint16_t temp;
};

struct si7021_data_t si7021_read();
//uint8_t si7021_read_user1();
//uint16_t si7021_read_temp();
//uint64_t si7021_read_sn();
// bit [7]: heater enable, bit [3:0]: heater current
//uint8_t si7021_write_heater(uint8_t heater);

#ifdef __cplusplus
}
#endif

#endif	// SI7021_H
