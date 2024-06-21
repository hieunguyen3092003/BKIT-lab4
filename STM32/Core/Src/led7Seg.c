/*
 * led7Seg.c
 *
 *  Created on: Apr 27, 2024
 *      Author: ngtrunghieu
 */

#include "led7Seg.h"
#include "spi.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

void initLed7Seg(void);
void led7SegTurnOff(uint8_t position);
void led7SegSetDigit(uint8_t num, uint8_t position, uint8_t show_dot);
void led7SegSetColon(uint8_t status);

void led7SegDisplay();

void led7SegDebugTurnOff(uint8_t index);
void led7SegDebugTurnOn(uint8_t index);

/* Variables */
static uint8_t led_7seg[4] = { 0, 1, 2, 3 };
static uint8_t led_7seg_map_of_output[10] = { 0x03, 0x9f, 0x25, 0x0d, 0x99, 0x49, 0x41,
		0x1f, 0x01, 0x09 };
static uint16_t led_7seg_index = 0;
static uint16_t spi_buffer = 0xffff;

/* Functions */
/**
 * @brief  	Init led 7 segment
 * @param  	None
 * @retval 	None
 */
void initLed7Seg()
{
	HAL_GPIO_WritePin(LD_LATCH_GPIO_Port, LD_LATCH_Pin, 1);
}

/**
 * @brief	Scan led 7 segment
 * @param	None
 * @note	Call in 1ms interrupt (Be called in default in Timer 4 callback function)
 * @retval 	None
 */
void led7SegDisplay()
{
	spi_buffer &= 0x00ff;
	spi_buffer |= led_7seg[led_7seg_index] << 8;

	switch (led_7seg_index) {
	case 0:
		spi_buffer |= 0x00b0;
		spi_buffer &= 0xffbf;
		break;
	case 1:
		spi_buffer |= 0x00d0;
		spi_buffer &= 0xffdf;
		break;
	case 2:
		spi_buffer |= 0x00e0;
		spi_buffer &= 0xffef;
		break;
	case 3:
		spi_buffer |= 0x0070;
		spi_buffer &= 0xff7f;
		break;
	default:
		break;
	}

	led_7seg_index = (led_7seg_index + 1) % 4;

	HAL_GPIO_WritePin(LD_LATCH_GPIO_Port, LD_LATCH_Pin, 0);
	HAL_SPI_Transmit(&hspi1, (void*) &spi_buffer, 2, 1);
	HAL_GPIO_WritePin(LD_LATCH_GPIO_Port, LD_LATCH_Pin, 1);
}

/**
 * @brief  	Display a digit at a position of led 7-segment
 * @param  	num	Number displayed
 * @param  	pos	The position displayed (index from 0)
 * @param  	show_dot Show dot in the led or not
 * @retval 	None
 */
void led7SegSetDigit(uint8_t num, uint8_t position, uint8_t show_dot)
{
	if (num <= 9)
	{
		led_7seg[position] = led_7seg_map_of_output[num] - show_dot;
	}
}

/**
 * @brief	Control the colon led
 * @param	status Status applied to the colon (1: turn on, 0: turn off)
 *          This parameter can be one of the following values:
 *				@arg 0: Turn off
 *				@arg 1: Turn on
 * @retval None
 */
void led7SegSetColon(uint8_t status)
{
	if (status == 1)
		spi_buffer &= ~(1 << 3);
	else
		spi_buffer |= (1 << 3);
}

/**
 *
 */
void led7SegTurnOff(uint8_t position)
{
	led_7seg[position] = 0xff;
}

/**
 * @brief  	Turn on one of three led at top-right corner of the maze
 * @param  	index Index of the led
 *          This parameter can be one of the following values:
 *            	@arg 6
 *            	@arg 7
 *            	@arg 8
 * @retval None
 */
void led7SegDebugTurnOn(uint8_t index)
{
	if (index >= 6 && index <= 8)
	{
		spi_buffer |= 1 << (index - 6);
	}
}

/**
 * @brief  	Turn off one of three led at top-right corner of the maze
 * @param  	index Index of the led
 *			This parameter can be one of the following values:
 *            	@arg 6
 *            	@arg 7
 *            	@arg 8
 * @retval None
 */
void led7SegDebugTurnOff(uint8_t index)
{
	if (index >= 6 && index <= 8)
	{
		spi_buffer &= ~(1 << (index - 6));
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
