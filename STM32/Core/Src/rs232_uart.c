/*
 * rs232_uart.c
 *
 *  Created on: Jul 8, 2024
 *      Author: hieun
 */

#include "rs232_uart.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Variables */
uint8_t receive_buffer1 = 0;
uint8_t msg[100];

Ring_Buffer rs232_rbuffer;
bool rs232_flag = false;

/* Functions */

/**
 * @brief Calculate power of a number
 *
 * @param base The base number
 * @param exponent The exponent
 * @return uint32_t The result of base^exponent
 */
uint32_t mypow(uint8_t base, uint8_t exponent)
{
    uint32_t result = 1;
    while (exponent > 0)
    {
        result *= base;
        exponent--;
    }
	return result;
}

/**
 * @brief Initialize UART for RS232 communication
 */
void initRS232()
{
    while (HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1) != HAL_OK)
    {
    	// For simplicity, we will just do an infinite loop here
    }
}

/**
 * @brief Send a string via UART
 *
 * @param str The string to send
 */
void rs232SendString(uint8_t *str)
{
	HAL_UART_Transmit(&huart1, (void*) msg, sprintf((void*) msg, "%s", str), 10);
}

/**
 * @brief Send bytes via UART
 *
 * @param bytes The byte array to send
 * @param size The size of the byte array
 */
void rs232SendBytes(uint8_t *bytes, uint16_t size)
{
	HAL_UART_Transmit(&huart1, bytes, size, 10);
}

/**
 * @brief Send a number as a string via UART
 *
 * @param num The number to send
 */
void rs232SendNum(uint32_t num)
{
	if (num == 0)
	{
		rs232SendString((void*)"0");
		return;
	}
	uint8_t num_flag = 0;
	int i;
	if (num < 0)
	{
		rs232SendString((void*)"-");
	}
	for (i = 10; i > 0; i--)
	{
		if ((num / mypow(10, i - 1)) != 0)
		{
			num_flag = 1;
			sprintf((void*) msg, "%ld", num / mypow(10, i - 1));
			rs232SendString(msg);
		}
		else
		{
			if (num_flag != 0)
				rs232SendString((void*)"0");
		}
		num %= mypow(10, i - 1);
	}
}


/**
 * @brief Send a number as a percentage string via UART
 *
 * @param num The number to send
 */
void rs232SendNumPercent(uint32_t num)
{
	sprintf((void*) msg, "%ld", num / 100);
	rs232SendString(msg);
	rs232SendString((void*)".");
	sprintf((void*) msg, "%ld", num % 100);
	rs232SendString(msg);
}

/**
 * @brief UART receive complete callback
 * @brief This function is evoked automatically when the UART peripheral received data send through uart and echo its back
 *
 * @param huart The UART handle
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		// rs232 isr
		if(RBufferInsert(&rs232_rbuffer, receive_buffer1))
		{
			rs232_flag = true;
		}

		// turn on the receice interrupt
		HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
	}
}

bool rs232GetFlag()
{
	if (rs232_flag)
	{
		rs232_flag = false;
		return true;
	}
	return false;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

