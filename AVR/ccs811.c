#include <util/delay.h>
#include <avr/io.h>
#include "i2c.h"
#include "ccs811.h"
#include <stdio.h>

// I2C address
#define CCS811_ADDRESS	0x5a
#define CCS_I2C_WRITE	((CCS_811_ADDRESS << 1) | 0)
#define CCS_I2C_READ	((CCS_811_ADDRESS << 1) | 1)
#define SI_I2C_ADDRESS  0X40
#define SI_I2C_WRITE	((SI_I2C_ADDRESS << 1) | 0)
#define SI_I2C_READ	((SI_I2C_ADDRESS << 1) | 1)

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
#define MEASURE_RH_HOLDMASTER	0xE5
#define MEASURE_T_HOLDMASTER 	0xE3

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

#define FLOAT2U16(v)	(uint16_t)(v * (float)(1U << 9))

uint16_t swap_u16(uint16_t data)
{
	return ((data & 0xff) << 8 | ((data >>8) & 0xff));
}

uint8_t ccs811_init()
{
	uint8_t seq[] = {0x11, 0xe5, 0x72, 0x8a};
	if (!i2c_write_data(CCS811_ADDRESS, SW_RESET, 4, seq)) {
		puts("Info: Error reseting sensor");
		return 0;
	}	
	_delay_ms(100);
		
	//Check app valid
	uint8_t status = CCS811_APP_VALID;
	if ((ccs811_read(STATUS_REG) & status) != status) {
		puts("Info: App not valid");
		return 0;
	}

	//Start Application
	while (!(ccs811_read(STATUS_REG) & CCS811_FW_MODE)) {
		printf("Info: Starting sensor...\n");
		if (!i2c_write_data(CCS811_ADDRESS, APP_START_REG, 0, 0))
			puts("Info: Error Starting Sensor");
	}
	
	uint8_t err = ccs811_read(ERR_ID_REG);
	printf("Info: Error: %02x\n", err);

	//Check status
	uint8_t check = CCS811_FW_MODE | CCS811_APP_VALID;
	if ((ccs811_read(STATUS_REG) & check) != check) {
		puts("Info: Sensor status error");
		return 0;
	}
	
	//Set measure mode
	uint8_t mode = 0x30;
	if(!i2c_write_data(CCS811_ADDRESS, MEAS_MODE_REG, 1, &mode)) {
		puts("Info: Set measure mode failed");
		return 0;
	}
	return 1;
}

uint8_t ccs811_read(uint8_t addr)
{
	uint8_t data;
	return i2c_read_data(CCS811_ADDRESS, addr, 1, &data) ? data : 0;
}

void ccs811_read_data(struct ccs811_data_t *ptr)
{
	i2c_read_data(CCS811_ADDRESS, ALG_RESULT_DATA, sizeof(struct ccs811_data_t), (void *)ptr);
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
	//uint8_t *ptr = (uint8_t *)&data;
	//printf("| ENVW:\t");
	//printf("0x%02x%02x\t", ptr[0], ptr[1]);
	//printf("0x%02x%02x\t", ptr[2], ptr[3]);
	return i2c_write_data(CCS811_ADDRESS, ENV_DATA, 4, (void *)&data);
}

uint8_t ccs811_poll()
{
	return ccs811_read(STATUS_REG) & CCS811_DATA_READY;
}

#if 0
uint8_t error()
{
	return err;
}

uint8_t ccs811_read(uint8_t addr)
{
	err = 0;
	i2c_start();						//start
	err += !i2c_write(CCS_I2C_WRITE);				//slave write address
	err += !i2c_write(addr);				//register

	i2c_start();						//restart
	err += !i2c_write(CCS_I2C_READ);				//slave read address
	if (err) {
		i2c_stop();
		return 0;
	}

	data = i2c_read(1);					//store data
	i2c_stop();						//stop
	puts("1");
	return err ? 0 : data;
}

uint8_t ccs811_write(uint8_t addr, uint8_t instructions)
{
//	puts(__FUNCTION__);
	err = 0;
	i2c_start();						//start
	err += !i2c_write(CCS_I2C_WRITE);				//WR
	err += !i2c_write(addr);				//register
	err += !i2c_write(instructions);				//instructions
	i2c_stop();						//stop
	return err ? 0 : data;
}

uint8_t ccs811_init()
{
	puts(__FUNCTION__);
	err = 0;	
	ccs811_read(HW_ID_REG);					//read HW_ID
	err += !(data == 0x81);					//check
	printf("HW_ID:%02x\n", data);
	ccs811_read(STATUS_REG);				//read status
	status = data;
	puts("123");
	err += !(status & _BV(4));				//check APP_VALID
	ccs811_write(APP_START_REG, 0);				//write APP_START
	ccs811_read(STATUS_REG);				//check status again
	status = data;
	printf("status code: %02x\n", status);
	err += !(status & 0x90);
	ccs811_write(MEAS_MODE_REG, DRIVE_MODE_1SEC);			//set mode, no int	
	if (err) {
		return 0;
	}

	puts("Sensor Initialise success");
	printf("error code: %02x\n\n", err);
	return 1;
}

static inline uint16_t swap_u16(uint16_t data)
{
	return ((data & 0xff) << 8 | ((data >> 8) & 0xff));	//little endianness
}

uint8_t device_read_multi(uint8_t device_w, uint8_t device_r, uint8_t addr, uint8_t len, uint8_t *ptr)
{
	//puts(__FUNCTION__);
	err = 0;
	i2c_start();
	err += !i2c_write(device_w);
	err += !i2c_write(addr);

	i2c_start();
	err += !i2c_write(device_r);
	if (err) {
		i2c_stop();
		return err;
	}

	while (len--)
		*ptr++ = i2c_read(len);
	i2c_stop();
	return err;
}

uint8_t device_write_multi(uint8_t device_w, uint8_t addr, uint8_t len, uint8_t *ptr)
{
	err = 0;
	i2c_start();
	err += !i2c_write(device_w);
	err += !i2c_write(addr);
	while (len--)
		err += !i2c_write(*ptr++);
	i2c_stop();
	return err;
}

uint8_t ccs811_polling(struct ccs811_data_t *ptr)
{
	device_read_multi(CCS_I2C_WRITE, CCS_I2C_READ, ALG_RESULT_DATA, sizeof(struct ccs811_data_t), (void *)ptr);
	ptr->eco2 = swap_u16(ptr->eco2);
	ptr->tvoc = swap_u16(ptr->tvoc);
	ptr->raw = swap_u16(ptr->raw);
	return 1;
}

uint8_t si7021_polling(struct si7021_data_t *pointer)
{
	device_read_multi(SI_I2C_WRITE, SI_I2C_READ, MEASURE_RH_HOLDMASTER, 2, (void *)&pointer->humidity);
	device_read_multi(SI_I2C_WRITE, SI_I2C_READ, MEASURE_T_HOLDMASTER, 2, (void *)&pointer->temperature);
	pointer->humidity = swap_u16(pointer->humidity);
	pointer->temperature = swap_u16(pointer->temperature);
	return 1;
}

uint8_t ccs811_poll()
{
	return ccs811_read(STATUS_REG) & 0x98;
}

uint8_t ccs811_calib(float humidity, float temperature)
{
	err = 0;
	static struct calibration data3;
	
	data3.rh = swap_u16(float2u16(humidity));
	data3.temp = swap_u16(float2u16(temperature + 25));
	err += !device_write_multi(CCS_I2C_WRITE, ENV_DATA, 4, (uint8_t *)&data3);
	return err;
}
#endif
