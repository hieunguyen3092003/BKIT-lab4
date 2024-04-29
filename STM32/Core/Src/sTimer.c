/*
 * sTimer.c
 *
 *  Created on: Apr 27, 2024
 *      Author: ngtrunghieu
 */

#include "sTimer.h"
#include "tim.h"
#include "led7Seg.h"

void initTimer2(void);
void initTimer4(void);

void sTimer2Set(uint16_t delay, uint16_t period);
void sTimer4Set(uint16_t delay, uint16_t period);

uint8_t sTimer2GetFlag(void);
uint8_t sTimer4GetFlag(void);

uint8_t timer_2_flag = 0;
uint16_t timer_2_counter = 0;
uint16_t timer_2_mul = 0;

uint8_t timer_4_flag = 0;
uint16_t timer_4_counter = 0;
uint16_t timer_4_mul = 0;

/**
 * @brief	init timer function
 */
void initTimer2()
{
	HAL_TIM_Base_Start_IT(&htim2);
}

void initTimer4()
{
	HAL_TIM_Base_Start_IT(&htim4);
}

/**
 * @brief	set timer duration(ms)
 * @param	duration(ms)
 */
void sTimer2Set(uint16_t delay, uint16_t period)
{
	timer_2_mul = period / TIMER_2_CYCLES;
	timer_2_counter = delay / TIMER_2_CYCLES;
	timer_2_flag = 0;
}

void sTimer4Set(uint16_t delay, uint16_t period)
{
	timer_4_mul = period / TIMER_4_CYCLES;
	timer_4_counter = delay / TIMER_4_CYCLES;
	timer_4_flag = 0;
}

/**
 * @brief	get flag timer fuction and auto reset flags and timer counter
 * @return	timer flag
 */
uint8_t sTimer2GetFlag()
{
	if(timer_2_flag)
	{
		timer_2_flag = 0;
		timer_2_counter = timer_2_mul;
		return 1;
	}
	return 0;
}

uint8_t sTimer4GetFlag()
{
	if(timer_4_flag)
	{
		timer_4_flag = 0;
		timer_4_counter = timer_4_mul;
		return 1;
	}
	return 0;
}
/**
 * @brief	callback function call every 1ms
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2)
	{
		if (timer_2_counter > 0)
		{
			--timer_2_counter;
		}
		if (timer_2_counter <= 0)
		{
			timer_2_flag = 1;
		}
	}

	if (htim->Instance == TIM4)
	{
		if (timer_4_counter > 0)
		{
			--timer_4_counter;
		}
		if (timer_4_counter <= 0)
		{
			timer_4_flag = 1;
		}
		led7SegDisplay();
	}
}
