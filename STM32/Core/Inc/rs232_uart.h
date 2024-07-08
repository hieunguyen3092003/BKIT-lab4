/*
 * rs232_uart.h
 *
 *  Created on: Jul 8, 2024
 *      Author: hieun
 */

#ifndef INC_RS232_UART_H_
#define INC_RS232_UART_H_

/* Includes */
#include "usart.h"

#include <stdio.h>
#include <stdint.h>
#include "utils.h"
#include "dataStructure.h"

/* Varialbes */
extern Ring_Buffer rs232_rbuffer;

/* Functions */
void initRS232(void);

void rs232SendString(uint8_t* str);
void rs232SendBytes(uint8_t* bytes, uint16_t size);
void rs232SendNum(uint32_t num);
void rs232SendNumPercent(uint32_t num);

bool rs232GetFlag(void);

#endif /* INC_RS232_UART_H_ */
