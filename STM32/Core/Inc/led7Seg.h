/*
 * led7Seg.h
 *
 *  Created on: Apr 27, 2024
 *      Author: ngtrunghieu
 */

#ifndef INC_LED7SEG_H_
#define INC_LED7SEG_H_

#include "stdint.h"

void initLed7Seg(void);
void led7SegTurnOff(uint8_t position);
void led7SegSetDigit(uint8_t num, uint8_t position, uint8_t show_dot);
void led7SegSetColon(uint8_t status);

void led7SegDisplay();

void led7SegDebugTurnOff(uint8_t index);
void led7SegDebugTurnOn(uint8_t index);

#endif /* INC_LED7SEG_H_ */
