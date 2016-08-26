#ifndef AM2302_H
#define AM2302_H

struct am2302_data
{
	uint16_t rh;
	uint16_t temp;
	uint8_t parity;
};

void am2302_init();
struct am2302_data am2302_read();
uint8_t am2302_check(struct am2302_data *ptr);

#endif	// AM2302_H
