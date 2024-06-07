/*
 * ds3231.c
 *
 *  Created on: Apr 27, 2024
 *      Author: ngtrunghieu
 */

#include "ds3231.h"
#include "i2c.h"
#include "utils.h"

//#ifdef __cplusplus
//extern "C"{
//#endif

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


#define DS3231_ADDRESS 0x68<<1

void initds3231(void);
void ds3231Write(uint8_t address, uint8_t value);
uint8_t ds3231Read(uint8_t address);
void ds3231ReadTime(void);
float ds3231ReadTemp(void);
void ds3231EnableA1(DS3231_State enable);
void ds3231EnableA2(DS3231_State enable);
void ds3231SetSec(uint8_t second);
void ds3231SetMin(uint8_t minute);
void ds3231SetHour(uint8_t hour);
void ds3231SetDay(uint8_t day);
void ds3231SetDate(uint8_t date);
void ds3231SetMonth(uint8_t month);
void ds3231SetYear(uint16_t year);
void ds3231SetSecA1(uint8_t second);
void ds3231SetMinA1(uint8_t minute);
void ds3231SetHourA1(uint8_t hour);
void ds3231SetDayA1(uint8_t day);
void ds3231SetDateA1(uint8_t date);
void ds3231SetMinA2(uint8_t minute);
void ds3231SetHourA2(uint8_t hour);
void ds3231SetDayA2(uint8_t day);
void ds3231SetDateA2(uint8_t date);
void ds3231ClearA1F(void);
void ds3231ClearA2F(void);
bool ds3231A1Trigger(void);
bool ds3231A2Trigger(void);

/* Variables */
Time current_time;
Time set_time;
Time set_alarm_1;
Time set_alarm_2;

uint8_t ds3231_buffer[19];

/**
 * @brief	init ds3231 real time clock micro controler
 */
void initds3231()
{
	while (HAL_I2C_IsDeviceReady(&hi2c1, DS3231_ADDRESS, 3, 50) != HAL_OK)
		;
}

/**
 * @brief	write data into specific address on ds3231 micro controller
 * @param	address Register address to write.
 * @param 	value Value (DECIMAL) to set, 1BYTE (0 to 255).
 */
void ds3231Write(uint8_t address, uint8_t value)
{
	uint8_t temp = DEC2BCD(value);
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, &temp, 1, 10);
}

/**
 * @brief	read 1 BYTE data from specific address on ds3231 micro controller
 * @return	DECIMAL value that store in specific address
 */
uint8_t ds3231Read(uint8_t address)
{
	uint8_t result;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, &result, 1, 10);
	return BCD2DEC(result);
}

/**
 * @brief	read 7 register (from reg 0x00 to reg 0x06) from ds3231 and store into array ds3231_buffer[]
 */
void ds3231ReadTime()
{
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT,
			ds3231_buffer, 7, 10);

	current_time.second = BCD2DEC(ds3231_buffer[0]);
	current_time.minute = BCD2DEC(ds3231_buffer[1]);
	current_time.hour = BCD2DEC(ds3231_buffer[2]);
	current_time.day = BCD2DEC(ds3231_buffer[3] & 0x7f);
	current_time.date = BCD2DEC(ds3231_buffer[4]);
	current_time.month = BCD2DEC(ds3231_buffer[5]);
	current_time.year = (BCD2DEC(ds3231_buffer[6]) + 2000) + (((ds3231_buffer[5] & 0b10000000) >> 7) * 100);
}

/**
 * @brief	ds3231 store temperature in register 11h (MSB) 12h (LSB)
 * @return	temperature
 */
float ds3231ReadTemp()
{
	float temperature = 0.0;
	uint8_t upper_byte_reg, lower_byte_reg;

	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, TEMP_MSB, I2C_MEMADD_SIZE_8BIT, &upper_byte_reg, 1, 10);
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, TEMP_LSB, I2C_MEMADD_SIZE_8BIT, &lower_byte_reg, 1, 10);

	if((upper_byte_reg & 0b10000000) != 0) // Negative number
	{
		temperature = -((~upper_byte_reg + 0b00000001) + ((lower_byte_reg >> 6) * 0.25));
	}
	else
	{
		temperature = upper_byte_reg + ((lower_byte_reg >> 6) * 0.25);
	}
	return temperature;
}

/*
 * @brief	set time functions
 * @param	valid time (DECIMAL) depends on function
 */
void ds3231SetSec(uint8_t second)
{
	ds3231Write(ADDRESS_SEC, second);
}
void ds3231SetMin(uint8_t minute)
{
	ds3231Write(ADDRESS_MIN, minute);
}
void ds3231SetHour(uint8_t hour)
{
	uint8_t hour_reg = DEC2BCD(hour) & 0b00111111;
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ADDRESS_HOUR, I2C_MEMADD_SIZE_8BIT, &hour_reg, 1, 10);
}
void ds3231SetDay(uint8_t day)
{
	ds3231Write(ADDRESS_DAY, day);
}
void ds3231SetDate(uint8_t date)
{
	ds3231Write(ADDRESS_DATE, date);
}
void ds3231SetMonth(uint8_t month)
{
	uint8_t century;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ADDRESS_MONTH, I2C_MEMADD_SIZE_8BIT,
				&century, 1, 10);
	uint8_t month_reg = DEC2BCD(month) | (century & 0b10000000); /* not interfere with century bit */
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ADDRESS_MONTH, I2C_MEMADD_SIZE_8BIT, &month_reg, 1, 10);
}
void ds3231SetYear(uint16_t year)
{
	uint8_t year_reg = DEC2BCD(year % 100);
	uint8_t century = (year / 100) % 20;
	uint8_t month_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ADDRESS_MONTH, I2C_MEMADD_SIZE_8BIT, &month_reg, 1, 10);
	month_reg = ((month_reg & 0b00011111) | (century << 7));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ADDRESS_MONTH, I2C_MEMADD_SIZE_8BIT, &month_reg, 1, 10);
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ADDRESS_YEAR, I2C_MEMADD_SIZE_8BIT, &year_reg, 1, 10);
}

/**
 * @brief Set alarm 1 mode. this function not interfere with data bit in register
 * @param alarmMode Alarm 1 mode, DS3231_A1_EVERY_S, DS3231_A1_MATCH_S, DS3231_A1_MATCH_S_M, DS3231_A1_MATCH_S_M_H, DS3231_A1_MATCH_S_M_H_DATE or DS3231_A1_MATCH_S_M_H_DAY.
 */
void DS3231_SetAlarm1Mode(DS3231_Alarm1Mode alarmMode)
{
	uint8_t temp_reg;

	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM1_SEC, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);
	temp_reg = ((temp_reg & 0b01111111) | (((alarmMode >> 0) & 0b00001111) << 7));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM1_SEC, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);

	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM1_MIN, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);
	temp_reg = ((temp_reg & 0b01111111) | (((alarmMode >> 1) & 0b00001111) << 7));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM1_MIN, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);

	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM1_HOUR, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);
	temp_reg = ((temp_reg & 0b00111111) | (((alarmMode >> 2) & 0b00001111) << 7));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM1_HOUR, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);

	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM1_DATE, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);
	temp_reg = ((temp_reg & 0b00111111) | (((alarmMode >> 3) & 0b00001111) << 7) | (alarmMode & 0b01000000));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM1_DATE, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);
}

/**
 * @brief Set alarm 2 mode. this function not interfere with data bit in register
 * @param alarmMode Alarm 2 mode, DS3231_A2_EVERY_M, DS3231_A2_MATCH_M, DS3231_A2_MATCH_M_H, DS3231_A2_MATCH_M_H_DATE or DS3231_A2_MATCH_M_H_DAY.
 */
void DS3231_SetAlarm2Mode(DS3231_Alarm2Mode alarmMode)
{
	uint8_t temp_reg;

	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM2_MIN, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);
	temp_reg = ((temp_reg & 0b01111111) | (((alarmMode >> 0) & 0b00001111) << 7));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM2_MIN, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);

	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM2_HOUR, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);
	temp_reg = ((temp_reg & 0b00111111) | (((alarmMode >> 2) & 0b00001111) << 7));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM2_HOUR, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);

	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM2_DATE, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);
	temp_reg = ((temp_reg & 0b00111111) | (((alarmMode >> 3) & 0b00001111) << 7) | (alarmMode & 0b01000000));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM2_DATE, I2C_MEMADD_SIZE_8BIT, &temp_reg, 1, 10);
}

/*
 * @brief	set ALARM 1 functions
 * @param	valid time alarm depends on function
 */
void ds3231SetSecA1(uint8_t second)
{
	uint8_t a1_sec_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM1_SEC, I2C_MEMADD_SIZE_8BIT, &a1_sec_reg, 1, 10);
	a1_sec_reg = ((DEC2BCD(second) & 0b01111111) | (a1_sec_reg & 0b10000000));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM1_SEC, I2C_MEMADD_SIZE_8BIT, &a1_sec_reg, 1, 10);
}
void ds3231SetMinA1(uint8_t minute)
{
	uint8_t a1_min_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM1_MIN, I2C_MEMADD_SIZE_8BIT, &a1_min_reg, 1, 10);
	a1_min_reg = ((DEC2BCD(minute) & 0b01111111) | (a1_min_reg & 0b10000000));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM1_MIN, I2C_MEMADD_SIZE_8BIT, &a1_min_reg, 1, 10);
}
void ds3231SetHourA1(uint8_t hour)
{
	uint8_t a1_hour_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM1_HOUR, I2C_MEMADD_SIZE_8BIT, &a1_hour_reg, 1, 10);
	a1_hour_reg = ((DEC2BCD(hour) & 0b00111111) | (a1_hour_reg & 0b10000000));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM1_HOUR, I2C_MEMADD_SIZE_8BIT, &a1_hour_reg, 1, 10);
}
void ds3231SetDayA1(uint8_t day)
{
	uint8_t a1_day_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM1_DATE, I2C_MEMADD_SIZE_8BIT, &a1_day_reg, 1, 10);
	a1_day_reg = ((DEC2BCD(day) & 0b00111111) | 0b01000000 | (a1_day_reg & 0b10000000));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM1_DATE, I2C_MEMADD_SIZE_8BIT, &a1_day_reg, 1, 10);
}
void ds3231SetDateA1(uint8_t date)
{
	uint8_t a1_date_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM1_DATE, I2C_MEMADD_SIZE_8BIT, &a1_date_reg, 1, 10);
	a1_date_reg = (DEC2BCD(date) & 0b00111111) | (a1_date_reg & 0b10000000);
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM1_DATE, I2C_MEMADD_SIZE_8BIT, &a1_date_reg, 1, 10);
}

/*
 * @brief	set ALARM 2 functions
 * @param	valid time alarm depends on function
 */
void ds3231SetMinA2(uint8_t minute)
{
	uint8_t a2_min_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM2_MIN, I2C_MEMADD_SIZE_8BIT, &a2_min_reg, 1, 10);
	a2_min_reg = ((DEC2BCD(minute) & 0b01111111) | (a2_min_reg & 0b10000000));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM2_MIN, I2C_MEMADD_SIZE_8BIT, &a2_min_reg, 1, 10);
}
void ds3231SetHourA2(uint8_t hour)
{
	uint8_t a2_hour_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM2_HOUR, I2C_MEMADD_SIZE_8BIT, &a2_hour_reg, 1, 10);
	a2_hour_reg = ((DEC2BCD(hour) & 0b00111111) | (a2_hour_reg & 0b10000000));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM2_HOUR, I2C_MEMADD_SIZE_8BIT, &a2_hour_reg, 1, 10);
}
void ds3231SetDayA2(uint8_t day)
{
	uint8_t a2_day_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM2_DATE, I2C_MEMADD_SIZE_8BIT, &a2_day_reg, 1, 10);
	a2_day_reg = ((DEC2BCD(day) & 0b00111111) | 0b01000000 | (a2_day_reg & 0b10000000));
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM2_DATE, I2C_MEMADD_SIZE_8BIT, &a2_day_reg, 1, 10);
}
void ds3231SetDateA2(uint8_t date)
{
	uint8_t a2_date_reg;
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, ALARM2_DATE, I2C_MEMADD_SIZE_8BIT, &a2_date_reg, 1, 10);
	a2_date_reg = (DEC2BCD(date) & 0b00111111) | (a2_date_reg & 0b10000000);
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, ALARM2_DATE, I2C_MEMADD_SIZE_8BIT, &a2_date_reg, 1, 10);
}

/*
 *
 */

void ds3231ClearA1F()
{

}
void ds3231ClearA2F()
{

}

/**
 *
 */
bool ds3231A1Trigger()
{
	return true;
}
bool ds3231A2Trigger()
{
	return true;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
