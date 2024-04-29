/*
 * sTimer.h
 *
 *  Created on: Apr 27, 2024
 *      Author: ngtrunghieu
 */

#ifndef INC_STIMER_H_
#define INC_STIMER_H_

#include <stdint.h>

#define TIMER_2_CYCLES		1
#define TIMER_4_CYCLES		1

void initTimer2(void);
void initTimer4(void);

void sTimer2Set(uint16_t delay, uint16_t period);
void sTimer4Set(uint16_t delay, uint16_t period);

uint8_t sTimer2GetFlag(void);
uint8_t sTimer4GetFlag(void);

#endif /* INC_STIMER_H_ */
