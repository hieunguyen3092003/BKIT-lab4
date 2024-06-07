/*
 * ds3231.h
 *
 *  Created on: Apr 27, 2024
 *      Author: ngtrunghieu
 */

#ifndef INC_DS3231_H_
#define INC_DS3231_H_

/* Includes */
#include <stdint.h>

/* Private define */
typedef uint8_t bool;
#define true 1
#define false 0

/* Register Constants */
#define ADDRESS_SEC			0x00
#define ADDRESS_MIN			0x01
#define ADDRESS_HOUR		0x02
#define ADDRESS_DAY			0x03
#define ADDRESS_DATE		0x04
#define ADDRESS_MONTH		0x05
#define ADDRESS_YEAR		0x06

#define ALARM1_SEC			0x07
#define ALARM1_MIN			0x08
#define ALARM1_HOUR			0x09
#define ALARM1_DATE			0x0a

#define ALARM2_MIN			0x0b
#define ALARM2_HOUR			0x0c
#define ALARM2_DATE			0x0d

#define DS3231_REG_CONTROL 	0x0e
#define DS3231_EOSC			7
#define DS3231_BBSQW		6
#define DS3231_CONV			5
#define DS3231_RS2			4
#define DS3231_RS1			3
#define DS3231_INTCN		2
#define DS3231_A2IE			1
#define DS3231_A1IE			0

#define DS3231_REG_STATUS	0x0f
#define DS3231_OSF			7
#define DS3231_EN32KHZ		3
#define DS3231_BSY			2
#define DS3231_A2F			1
#define DS3231_A1F			0

#define DS3231_AGING		0x10

#define TEMP_MSB			0x11
#define TEMP_LSB			0x12

/* Variables */
extern uint8_t ds3231_hours;
extern uint8_t ds3231_min;
extern uint8_t ds3231_sec;
extern uint8_t ds3231_date;
extern uint8_t ds3231_day;
extern uint8_t ds3231_month;
extern uint8_t ds3231_year;

/*----------------------------------------------------------------------------*/

typedef struct
{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint16_t year;
}Time;
extern Time current_time;
extern Time set_time;
extern Time set_alarm_1;
extern Time set_alarm_2;

/*
 * This variable is used for deeper interfere this lab currently not used
 * please view this site for more intel: https://github.com/hieunguyen3092003/BKIT-ExampleDS3231.git
 */
typedef enum DS3231_Rate{
	DS3231_1HZ,
	DS3231_1024HZ,
	DS3231_4096HZ,
	DS3231_8192HZ
}DS3231_Rate;

typedef enum DS3231_InterruptMode
{
	DS3231_SQUARE_WAVE_INTERRUPT,
	DS3231_ALARM_INTERRUPT
}DS3231_InterruptMode;

typedef enum DS3231_State
{
	DS3231_DISABLED,
	DS3231_ENABLED
}DS3231_State;

typedef enum D3231_Alarm1Mode
{
	DS3231_A1_EVERY_S = 0x0f,
	DS3231_A1_MATCH_S = 0x0e,
	DS3231_A1_MATCH_S_M = 0x0c,
	DS3231_A1_MATCH_S_M_H = 0x08,
	DS3231_A1_MATCH_S_M_H_DATE = 0x00,
	DS3231_A1_MATCH_S_M_H_DAY = 0x40
}DS3231_Alarm1Mode;

typedef enum D3231_Alarm2Mode
{
	DS3231_A2_EVERY_M = 0x07,
	DS3231_A2_MATCH_M = 0x06,
	DS3231_A2_MATCH_M_H = 0x04,
	DS3231_A2_MATCH_M_H_DATE = 0x00,
	DS3231_A2_MATCH_M_H_DAY = 0x40
}DS3231_Alarm2Mode;

/* Functions */
void initds3231(void);
void ds3231ReadTime(void);
float ds3231ReadTemp(void);
void ds3231SetSec(uint8_t second);
void ds3231SetMin(uint8_t minute);
void ds3231SetHour(uint8_t hour);
void ds3231SetDay(uint8_t day);
void ds3231SetDate(uint8_t date);
void ds3231SetMonth(uint8_t month);
void ds3231SetYear(uint16_t year);


#endif /* INC_DS3231_H_ */
