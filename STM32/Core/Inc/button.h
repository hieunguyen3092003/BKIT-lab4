/*
 * button.h
 *
 *  Created on: Jun 3, 2024
 *      Author: hieun
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>
#include "spi.h"
#include "gpio.h"

/* Variables */
extern uint16_t button_count[16];

/* Functions */
void initButton(void);
void buttonScan(void);

#endif /* INC_BUTTON_H_ */
