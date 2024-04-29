/*
 * ds3231.c
 *
 *  Created on: Apr 27, 2024
 *      Author: ngtrunghieu
 */

#include "ds3231.h"
#include "i2c.h"
#include "utils.h"

#define DS3231_ADDRESS 0x68<<1

void initds3231(void);
void ds3231Write(uint8_t address, uint8_t value);
uint8_t ds3231Read(uint8_t address);
void ds3231ReadTime(void);

/* Variables */
uint8_t ds3231_buffer[7];

uint8_t ds3231_hours = 1;
uint8_t ds3231_min = 1;
uint8_t ds3231_sec = 1;
uint8_t ds3231_date = 1;
uint8_t ds3231_day = 1;
uint8_t ds3231_month = 1;
uint8_t ds3231_year = 1;

/**
 * @brief	init ds3231 real time clock micro controler
 * @param	initial param for second minute hour day date month year
 */
void initds3231()
{
	while (HAL_I2C_IsDeviceReady(&hi2c1, DS3231_ADDRESS, 3, 50) != HAL_OK)
		;

	ds3231_buffer[0] = DEC2BCD(ds3231_sec); //second
	ds3231_buffer[1] = DEC2BCD(ds3231_min); //minute
	ds3231_buffer[2] = DEC2BCD(ds3231_hours); //hour
	ds3231_buffer[3] = DEC2BCD(ds3231_day);  //day
	ds3231_buffer[4] = DEC2BCD(ds3231_date); //date
	ds3231_buffer[5] = DEC2BCD(ds3231_month);  //month
	ds3231_buffer[6] = DEC2BCD(ds3231_year); //year
}

/**
 * @brief	write data into specific address on ds3231 micro controller
 */
void ds3231Write(uint8_t address, uint8_t value)
{
	uint8_t temp = DEC2BCD(value);
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, address, I2C_MEMADD_SIZE_8BIT,
			&temp, 1, 10);
}

/**
 * @brief	read data from specific address on ds3231 micro controller
 * @return	decimal value
 */
uint8_t ds3231Read(uint8_t address)
{
	uint8_t result;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT,
			&result, 1, 10);
	return BCD2DEC(result);
}

/**
 * @brief	read data from ds3231 and store into array ds3231_buffer[]
 */
void ds3231ReadTime()
{
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT,
			ds3231_buffer, 7, 10);

	ds3231_sec = BCD2DEC(ds3231_buffer[0]);
	ds3231_min = BCD2DEC(ds3231_buffer[1]);
	ds3231_hours = BCD2DEC(ds3231_buffer[2]);
	ds3231_day = BCD2DEC(ds3231_buffer[3]);
	ds3231_date = BCD2DEC(ds3231_buffer[4]);
	ds3231_month = BCD2DEC(ds3231_buffer[5]);
	ds3231_year = BCD2DEC(ds3231_buffer[6]);
}
