/*
 * dataStructure.h
 *
 *  Created on: Jul 8, 2024
 *      Author: hieun
 */

#ifndef INC_DATASTRUCTURE_H_
#define INC_DATASTRUCTURE_H_

#include <stdint.h>

/* Private define */
#define BUFFER_SIZE 100

typedef uint8_t bool;
#define true 1
#define false 0

typedef struct
{
	uint8_t buffer[BUFFER_SIZE];
	volatile uint8_t head_index;
	volatile uint8_t tail_index;
} Ring_Buffer;

/* Functions */
void initRBuffer(Ring_Buffer *pRBuffer);

bool RBufferIsEmpty(Ring_Buffer *pRBuffer);
bool RBufferInsert(Ring_Buffer *pRBuffer, uint8_t data);
bool RBufferRead(Ring_Buffer *pRBuffer, uint8_t *data);

#endif /* INC_DATASTRUCTURE_H_ */
