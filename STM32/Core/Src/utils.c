/*
 * utils.c
 *
 *  Created on: Apr 27, 2024
 *      Author: ngtrunghieu
 */

#include "utils.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

uint8_t BCD2DEC(uint8_t data);
uint8_t DEC2BCD(uint8_t data);

/**
 * @brief: transform splited 8 bit (4 bit MSB represent tens and 4 bit LSB represent units) to decimal
 * @param: splited 8 bit
 */
uint8_t BCD2DEC(uint8_t data)
{
	return (data >> 4) * 10 + (data & 0x0f);
}

/**
 * brief: transform decimal to splited 8 bit (4 bit MSB represent tens and 4 bit LSB represent units)
 * @param: decimal
 */
uint8_t DEC2BCD(uint8_t data)
{
	return (data / 10) << 4 | (data % 10);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
