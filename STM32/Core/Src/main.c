/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sTimer.h"
#include "ds3231.h"
#include "lcd.h"
#include "led7Seg.h"
#include "button.h"
#include <math.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PI 3.14159265358979323846

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void debugSystem(void);
void initSystem(void);
void displayClock(int x_coor, int y_coor, int radius);
void displayLocation(int y_coor, char *str, char *GMT_str, uint8_t char_size);
void displayClockWise(int x_coor, int y_coor, int radius, const uint8_t *second, const uint8_t *minute, const uint8_t *hour, uint16_t color_sec_clw, uint16_t color_min_clw, uint16_t color_hour_clw);
void displayTime(int x_coor, int y_coor, const uint8_t *second, const uint8_t *minute, const uint8_t *hour, const uint8_t *day, const uint8_t *date, const uint8_t *month, const uint16_t *year, float temperature,
		uint8_t char_size, uint8_t subchar_size, uint16_t color_sec, uint16_t color_min, uint16_t color_hour, uint16_t color_day, uint16_t color_date, uint16_t color_month, uint16_t color_year);
void displayTimeLed7Seg(const uint8_t *second, const uint8_t *minute, const uint8_t *hour);
void setTime(uint8_t *second, uint8_t *minute, uint8_t *hour, uint8_t *day, uint8_t *date, uint8_t *month, uint16_t *year);
void setAlarm1(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day, uint8_t date);
void setAlarm2(uint8_t minute, uint8_t hour, uint8_t day, uint8_t date);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
enum State
{
    Mode_init,
    Mode_word_clock,
	Mode_login,
    Mode_config_time,
    Mode_config_alarm,
	Mode_stopwatch,
	Mode_timers,
	Mode_monitor_register
};
enum State current_mode = Mode_init;
enum State previous_mode = Mode_init;

enum State_config
{
	Mode_config_second,
	Mode_config_minute,
	Mode_config_hour,
	Mode_config_day,
	Mode_config_date,
	Mode_config_month,
	Mode_config_year
};
enum State_config current_mode_config = Mode_config_second;
enum State_config previous_mode_config = Mode_config_minute;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  initSystem();

  sTimer4Set(1000, 50); // interrupt every 50ms
  sTimer2Set(0, 500); // interrupt every 500ms

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(sTimer4GetFlag())
	  {
		  buttonScan();
		  led7SegDisplay();
	  }

	  switch (current_mode)
	  {
	  case Mode_init:
	  {
		  led7SegSetColon(1);
		  setTime(&set_time.second, &set_time.minute, &set_time.hour, &set_time.day, &set_time.date, &set_time.month, &set_time.year);

		  current_time.alarm_on = false;

		  current_mode = Mode_config_time;
		  break;
	  }
	  case Mode_word_clock:
	  {
		  if(previous_mode != current_mode)
		  {
			  lcdClear(WHITE);
			  displayClock(LCD_WIDTH / 2, 110, 100);
//			  displayLocation((240 + ((320 - 240) / 2) + 20), "HaNoi", "GMT +7", 24);
			  previous_mode = current_mode;
		  }

		  if(sTimer2GetFlag())
		  {
			  debugSystem();
			  ds3231ReadTime();

			  displayClockWise(LCD_WIDTH / 2, 110, 100, &current_time.second, &current_time.minute, &current_time.hour, BLUE, RED, BLACK);
			  displayTime(LCD_WIDTH / 2, 240, &current_time.second, &current_time.minute, &current_time.hour, &current_time.day, &current_time.date, &current_time.month, &current_time.year, ds3231ReadTemp(), 32, 24, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK);
			  displayTimeLed7Seg(&current_time.second, &current_time.minute, &current_time.hour);
		  }

		  break;
	  }
	  case Mode_config_time:
	  {
		  if(previous_mode != current_mode)
		  {
			  ds3231ReadTime();

			  set_time.second = current_time.second;
			  set_time.minute = current_time.minute;
			  set_time.hour = current_time.hour;
			  set_time.day = current_time.day;
			  set_time.date = current_time.date;
			  set_time.month = current_time.month;
			  set_time.year = current_time.year;

			  lcdClear(WHITE);
			  displayClock(LCD_WIDTH / 2, 110, 100);

			  previous_mode = current_mode;
		  }

		  switch (current_mode_config)
		  {
			case Mode_config_second:
			{
				if(previous_mode_config != current_mode_config)
				{
					displayClockWise(LCD_WIDTH / 2, 110, 100, &set_time.second, &set_time.minute, &set_time.hour, RED, LIGHTBLUE, LIGHTBLUE);
					displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					displayTime(LCD_WIDTH / 2, 240, &set_time.second, &set_time.minute, &set_time.hour, &set_time.day, &set_time.date, &set_time.month, &set_time.year, ds3231ReadTemp(), 32, 24, RED, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK);

					previous_mode_config = current_mode_config;
				}

				if(sTimer2GetFlag())
				{
					static int counter = 0;
					counter += 1;

					if(counter % 4 == 0)
					{
						led7SegDebugTurnOff(6);
						led7SegDebugTurnOff(7);
						led7SegDebugTurnOff(8);
					}
					else if(counter % 2 == 0)
					{
						displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					}
				}

				if(button_count[11] % 30 == 1) // check button is held 1.5 second
				{
					current_mode_config = Mode_config_minute;
				}

				break;
			}
			case Mode_config_minute:
			{
				if(previous_mode_config != current_mode_config)
				{
					displayClockWise(LCD_WIDTH / 2, 110, 100, &set_time.second, &set_time.minute, &set_time.hour, LIGHTBLUE, RED, LIGHTBLUE);
					displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					displayTime(LCD_WIDTH / 2, 240, &set_time.second, &set_time.minute, &set_time.hour, &set_time.day, &set_time.date, &set_time.month, &set_time.year, ds3231ReadTemp(), 32, 24, BLACK, RED, BLACK, BLACK, BLACK, BLACK, BLACK);

					previous_mode_config = current_mode_config;
				}

				if(sTimer2GetFlag())
				{
					static int counter = 0;
					counter += 1;

					if(counter % 4 == 0)
					{
						led7SegTurnOff(2);
						led7SegTurnOff(3);
					}
					else if(counter % 2 == 0)
					{
						displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					}
				}

				if(button_count[11] % 30 == 1) // check button is held 1.5 second
				{
					current_mode_config = Mode_config_hour;
				}
				else if(button_count[15] % 30 == 1)
				{
					current_mode_config = Mode_config_second;
				}

				break;
			}
			case Mode_config_hour:
			{
				if(previous_mode_config != current_mode_config)
				{
					displayClockWise(LCD_WIDTH / 2, 110, 100, &set_time.second, &set_time.minute, &set_time.hour, LIGHTBLUE, LIGHTBLUE, RED);
					displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					displayTime(LCD_WIDTH / 2, 240, &set_time.second, &set_time.minute, &set_time.hour, &set_time.day, &set_time.date, &set_time.month, &set_time.year, ds3231ReadTemp(), 32, 24, BLACK, BLACK, RED, BLACK, BLACK, BLACK, BLACK);

					previous_mode_config = current_mode_config;
				}

				if(sTimer2GetFlag())
				{
					static int counter = 0;
					counter += 1;

					if(counter % 4 == 0)
					{
						led7SegTurnOff(0);
						led7SegTurnOff(1);
					}
					else if(counter % 2 == 0)
					{
						displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					}
				}

				if(button_count[11] % 30 == 1) // check button is held 1.5 second
				{
					current_mode_config = Mode_config_day;
				}
				else if(button_count[15] % 30 == 1)
				{
					current_mode_config = Mode_config_minute;
				}

				break;
			}
			case Mode_config_day:
			{
				if(previous_mode_config != current_mode_config)
				{
					displayClockWise(LCD_WIDTH / 2, 110, 100, &set_time.second, &set_time.minute, &set_time.hour, LIGHTBLUE, LIGHTBLUE, LIGHTBLUE);
					displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					displayTime(LCD_WIDTH / 2, 240, &set_time.second, &set_time.minute, &set_time.hour, &set_time.day, &set_time.date, &set_time.month, &set_time.year, ds3231ReadTemp(), 32, 24, BLACK, BLACK, BLACK, RED, BLACK, BLACK, BLACK);

					previous_mode_config = current_mode_config;
				}

				if(button_count[11] % 30 == 1) // check button is held 1.5 second
				{
					current_mode_config = Mode_config_date;
				}
				else if(button_count[15] % 30 == 1)
				{
					current_mode_config = Mode_config_hour;
				}

				break;
			}
			case Mode_config_date:
			{
				if(previous_mode_config != current_mode_config)
				{
					displayClockWise(LCD_WIDTH / 2, 110, 100, &set_time.second, &set_time.minute, &set_time.hour, LIGHTBLUE, LIGHTBLUE, LIGHTBLUE);
					displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					displayTime(LCD_WIDTH / 2, 240, &set_time.second, &set_time.minute, &set_time.hour, &set_time.day, &set_time.date, &set_time.month, &set_time.year, ds3231ReadTemp(), 32, 24, BLACK, BLACK, BLACK, BLACK, RED, BLACK, BLACK);

					previous_mode_config = current_mode_config;
				}

				if(button_count[11] % 30 == 1) // check button is held 1.5 second
				{
					current_mode_config = Mode_config_month;
				}
				else if(button_count[15] % 30 == 1)
				{
					current_mode_config = Mode_config_day;
				}

				break;
			}
			case Mode_config_month:
			{
				if(previous_mode_config != current_mode_config)
				{
					displayClockWise(LCD_WIDTH / 2, 110, 100, &set_time.second, &set_time.minute, &set_time.hour, LIGHTBLUE, LIGHTBLUE, LIGHTBLUE);
					displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					displayTime(LCD_WIDTH / 2, 240, &set_time.second, &set_time.minute, &set_time.hour, &set_time.day, &set_time.date, &set_time.month, &set_time.year, ds3231ReadTemp(), 32, 24, BLACK, BLACK, BLACK, BLACK, BLACK, RED, BLACK);

					previous_mode_config = current_mode_config;
				}

				if(button_count[11] % 30 == 1) // check button is held 1.5 second
				{
					current_mode_config = Mode_config_year;
				}
				else if(button_count[15] % 30 == 1)
				{
					current_mode_config = Mode_config_date;
				}

				break;
			}
			case Mode_config_year:
			{
				if(previous_mode_config != current_mode_config)
				{
					displayClockWise(LCD_WIDTH / 2, 110, 100, &set_time.second, &set_time.minute, &set_time.hour, LIGHTBLUE, LIGHTBLUE, LIGHTBLUE);
					displayTimeLed7Seg(&set_time.second, &set_time.minute, &set_time.hour);
					displayTime(LCD_WIDTH / 2, 240, &set_time.second, &set_time.minute, &set_time.hour, &set_time.day, &set_time.date, &set_time.month, &set_time.year, ds3231ReadTemp(), 32, 24, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, RED);

					previous_mode_config = current_mode_config;
				}

				if(button_count[15] % 30 == 1)
				{
					current_mode_config = Mode_config_month;
				}

				break;
			}
		}

		if(button_count[12] >= 1)
		{
		  setTime(&set_time.second, &set_time.minute, &set_time.hour, &set_time.day, &set_time.date, &set_time.month, &set_time.year);
		  current_mode = Mode_word_clock;
		}
		else if(button_count[14] >= 1)
		{
		  current_mode = Mode_word_clock;
		}

		break;
	  }
	  case Mode_config_alarm:
	  {
		  break;
	  }
	  case Mode_stopwatch:
	  {
		  break;
	  }
	  case Mode_timers:
	  {
		  break;
	  }
	  case Mode_monitor_register:
	  {
		  break;
	  }
	  default:
	  {
		  current_mode = Mode_init;
	  }
	  }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void debugSystem()
{
	HAL_GPIO_TogglePin(GPIOE, LED_DEBUG_Pin);
}
void initSystem()
{
	initTimer2();
	initTimer4();
	initLCD();
	initLed7Seg();
	initds3231();
	initButton();
}
void setTime(uint8_t *second, uint8_t *minute, uint8_t *hour, uint8_t *day, uint8_t *date, uint8_t *month, uint16_t *year)
{
	ds3231SetSec(*second);
	ds3231SetMin(*minute);
	ds3231SetHour(*hour);
	ds3231SetDay(*day);
	ds3231SetDate(*date);
	ds3231SetMonth(*month);
	ds3231SetYear(*year);
}

void displayClock(int x_coor, int y_coor, int radius)
{
    const uint8_t char_size = 24;

    lcdDrawCircle(x_coor, y_coor, BLUE, radius + 2, 1);
	lcdDrawCircle(x_coor, y_coor, WHITE, radius, 1);

    for (int i = 0; i < 12; i++)
    {
        float angle = (i * 30) * (PI / 180); // Convert angle to radians
        int x = x_coor + (radius - 15) * sin(angle);
        int y = y_coor + (radius - 15) * -cos(angle);

        lcdShowIntNumCenter(x, y, ((i == 0) ? 12 : i), 2, BLACK, WHITE, char_size, 1);
    }
}

/*
 * @param	x_coor, y_coor coordinate of circle center
 * @param 	radius radius of a clock
 * @param	*second, *minute, *hour pointer to variable time store on struct Time
 * @param 	color_sec_clw, color_min_clw, color_hour_clw color of second, minute, hour clockwise
 */
void displayClockWise(int x_coor, int y_coor, int radius, const uint8_t *second, const uint8_t *minute, const uint8_t *hour, uint16_t color_sec_clw, uint16_t color_min_clw, uint16_t color_hour_clw)
{
	static float angle_sec, angle_min, angle_hour;
    int x, y;

    x = x_coor + (radius - 30) * sin(angle_sec);
    y = y_coor + (radius - 30) * -cos(angle_sec);
    lcdDrawLine(x_coor, y_coor, x, y, WHITE);

    angle_sec = (*second * 6) * (PI / 180);
    x = x_coor + (radius - 30) * sin(angle_sec);
    y = y_coor + (radius - 30) * -cos(angle_sec);
    lcdDrawLine(x_coor, y_coor, x, y, color_sec_clw);

    if(angle_min != angle_sec)
    {
        x = x_coor + (radius - 40) * sin(angle_min);
        y = y_coor + (radius - 40) * -cos(angle_min);
        lcdDrawLine(x_coor, y_coor, x, y, WHITE);
    }
    angle_min = (*minute * 6) * (PI / 180);
    x = x_coor + (radius - 40) * sin(angle_min);
    y = y_coor + (radius - 40) * -cos(angle_min);
    lcdDrawLine(x_coor, y_coor, x, y, color_min_clw);

    if(angle_hour != angle_sec && angle_hour != angle_min)
    {
        x = x_coor + (radius - 50) * sin(angle_hour);
        y = y_coor + (radius - 50) * -cos(angle_hour);
        lcdDrawLine(x_coor, y_coor, x, y, WHITE);
    }
    angle_hour = ((*hour % 12 + *minute / 60.0) * 30) * (PI / 180); // 360 degrees / 12 hours = 30 degrees per hour
    x = x_coor + (radius - 50) * sin(angle_hour);
    y = y_coor + (radius - 50) * -cos(angle_hour);
    lcdDrawLine(x_coor, y_coor, x, y, color_hour_clw);
}

void displayTime(int x_coor, int y_coor, const uint8_t *second, const uint8_t *minute, const uint8_t *hour, const uint8_t *day, const uint8_t *date, const uint8_t *month, const uint16_t *year, float temperature,
		uint8_t char_size, uint8_t subchar_size, uint16_t color_sec, uint16_t color_min, uint16_t color_hour, uint16_t color_day, uint16_t color_date, uint16_t color_month, uint16_t color_year)
{
	lcdShowIntNumCenter(x_coor - char_size * 2, y_coor, *hour, 2, color_hour, WHITE, char_size, 0);
	lcdShowIntNumCenter(x_coor, y_coor, *minute, 2, color_min, WHITE, char_size, 0);
	lcdShowIntNumCenter(x_coor + char_size * 2 , y_coor, *second, 2, color_sec, WHITE, char_size, 0);

	lcdShowStringCenter(x_coor + char_size, y_coor, ":", BLACK, WHITE, char_size, 1);
	lcdShowStringCenter(x_coor - (char_size * 2) + char_size, y_coor, ":", BLACK, WHITE, char_size, 1);

//	if(*hour == 0 && *minute == 0 && *second == 0) // this case is false with settime
//	{
	lcdShowIntNumCenter(x_coor - subchar_size * 2, y_coor + char_size, *date, 2, color_date, WHITE, subchar_size, 0);
	lcdShowIntNumCenter(x_coor, y_coor + char_size, *month, 2, color_month, WHITE, subchar_size, 0);
	lcdShowIntNumCenter(x_coor + subchar_size * 2 + subchar_size / 2, y_coor + char_size, *year, 4, color_year, WHITE, subchar_size, 0);

	lcdShowStringCenter(x_coor + subchar_size, y_coor + char_size, "/", BLACK, WHITE, subchar_size, 1);
	lcdShowStringCenter(x_coor - (subchar_size * 2) + subchar_size, y_coor + char_size, "/", BLACK, WHITE, subchar_size, 1);
//	}

	return;
}

void displayLocation(int y_coor, char *str, char *GMT_str, uint8_t char_size)
{
	lcdShowString(20, y_coor - char_size / 2, str, BLACK, WHITE, char_size, 1);
	lcdShowString(240 - 20 - (strlen(GMT_str) * (char_size / 2)), y_coor - char_size / 2, GMT_str, BLACK, WHITE, char_size, 1);
	return;
}

void displayTimeLed7Seg(const uint8_t *second, const uint8_t *minute, const uint8_t *hour)
{
	if(((*second / 10) >> 0) & 0x01)
	{
		led7SegDebugTurnOn(6);
	}
	else
	{
		led7SegDebugTurnOff(6);
	}
	if(((*second / 10) >> 1) & 0x01)
	{
		led7SegDebugTurnOn(7);
	}
	else
	{
		led7SegDebugTurnOff(7);
	}
	if(((*second / 10) >> 2) & 0x01)
	{
		led7SegDebugTurnOn(8);
	}
	else
	{
		led7SegDebugTurnOff(8);
	}

	led7SegSetDigit(*hour / 10, 0, 0);
	led7SegSetDigit(*hour % 10, 1, 0);

	led7SegSetDigit(*minute / 10, 2, 0);
	led7SegSetDigit(*minute % 10, 3, 0);

	return;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
