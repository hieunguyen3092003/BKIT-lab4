/*
 * dataStructure.c
 *
 *  Created on: Jul 8, 2024
 *      Author: hieun
 */

#include "dataStructure.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief init ring buffer called in init system
 * @param *pRBuffer pointer to ring buffer
 */
void initRBuffer(Ring_Buffer *pRBuffer)
{
	pRBuffer->head_index = 0;
	pRBuffer->tail_index = 0;
}

bool RBufferIsEmpty(Ring_Buffer *pRBuffer)
{
	return (pRBuffer->head_index == pRBuffer->tail_index);
}

bool RBufferIsFull(Ring_Buffer *pRBuffer)
{
	return ((pRBuffer->head_index + 1) % BUFFER_SIZE == pRBuffer->tail_index);
}

/**
 * @brief insert data into the head of ring buffer
 * @param *pRBuffer pointer to Ring_Buffer wants to write
 * @param data 1 byte data wants to write into ring buffer
 * @retval return true if data is writen successfully
 */
bool RBufferInsert(Ring_Buffer *pRBuffer, uint8_t data)
{
	if(RBufferIsFull(pRBuffer))
	{
		// ring buffer is full
		return false;
	}
	pRBuffer->buffer[pRBuffer->head_index] = data;
	pRBuffer->head_index = (pRBuffer->head_index + 1) % BUFFER_SIZE;
	return true;
}

/**
 * @brief read bytes at the tail of the buffer(oldest byte)
 * @param *pRBuffer pointer to ring buffer wants to read
 * @param *pData pointer to a uint8_t varialble to store data into
 * @retval true if data is read successfully
 */
bool RBufferRead(Ring_Buffer *pRBuffer, uint8_t *pData)
{
	if(RBufferIsEmpty(pRBuffer))
	{
		// ring buffer is empty
		return false;
	}
	*pData = pRBuffer->buffer[pRBuffer->tail_index];
	pRBuffer->tail_index = (pRBuffer->tail_index + 1) % BUFFER_SIZE;
	return true;
}

#ifdef __cplusplus
}
#endif

