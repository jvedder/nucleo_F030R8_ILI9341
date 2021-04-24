/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "FT6206.h"
#include <stdio.h>
#include <string.h>
//#include <stdarg.h> //for va_list var arg functions

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//void myprintf(const char *fmt, ...);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if 0
void myprintf(const char *fmt, ...)
{
  static char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  int len = strlen(buffer);
  HAL_UART_Transmit(&huart2, (uint8_t*)buffer, len, -1);

}
#endif

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(1000);

  printf("\r\n\r\nPOR\r\n");
  printf("Build: " __DATE__ ", " __TIME__ "\r\n");

  LCD_Init();
  LCD_FillScreen(ILI9341_WHITE);
  LCD_cursor_x = 0;
  LCD_cursor_y = 40;
  LCD_textcolor = ILI9341_BLACK;
  LCD_Font = &FreeMono12pt7b;
  //  LCD_Font = &FreeMono12pt7b;
//  LCD_DrawText( (const uint8_t *)"Mono\n" );
//  LCD_Font = &FreeSans12pt7b;
//  LCD_DrawText( (const uint8_t *)"Sans\n" );
//  LCD_Font = &FreeSerif12pt7b;
//  LCD_DrawText( (const uint8_t *)"Serif\n" );

  char text[64];
  //LCD_WriteFillRectPreclipped(10, 20, 30, 40, ILI9341_RED);
  //LCD_DrawHLine(41,60, 30, ILI9341_BLUE );
  //LCD_DrawVLine(40,61, 40, ILI9341_GREEN );
  //LCD_DrawLine(1,1, 100, 200, ILI9341_WHITE);

  FT6206_init(40);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  flag = 0;
  uint8_t rot= 3;
  LCD_SetRotation(rot);
  FT6206_setRotation(rot);

  while (1)
  {
      if (HAL_GPIO_ReadPin(USER_BTN_GPIO_Port, USER_BTN_Pin) != GPIO_PIN_SET)
      {
          rot = (rot+1) % 4;
          LCD_SetRotation(rot);
          FT6206_setRotation(rot);
          LCD_FillScreen(ILI9341_WHITE);
          LCD_cursor_x = 0;
          LCD_cursor_y = LCD_Font->yAdvance;
          sprintf(text, "Rotation %d", rot);
          printf("%s\r\n", text);
          LCD_DrawText( (uint8_t *) text);
      }

      FT6206_readData();
      if (FT6206_touched)
      {
          sprintf(text, "%3d,%3d", FT6206_touchX, FT6206_touchY);
          printf("%s\r\n", text);
          LCD_cursor_x = 0;
          LCD_cursor_y = 2 * LCD_Font->yAdvance;
          LCD_DrawFillRect(LCD_cursor_x, LCD_cursor_y, 120,-LCD_Font->yAdvance, ILI9341_WHITE);
          LCD_DrawText( (uint8_t *) text);
      }

#if 0
      //printf("------------------\r\n");
      FT6206_readData();
      if (FT6206_touched)
      {
          printf("(%d,%d)\r\n", FT6206_touchX, FT6206_touchY);
          flag = 0;
      }
      else if (flag == 0)
      {
          printf("Not Touched\r\n");
          flag = 1;
      }
#endif

#if 0
      if (HAL_GPIO_ReadPin(TS_INT_GPIO_Port, TS_INT_Pin))
      {
          printf("*\r\n");
      }
      else
      {
          printf(".\r\n");
      }
#endif

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

int __io_putchar(int ch)
{
    /* Write a character to the UART and block until transmitted */
    HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

int __io_getchar(void)
{
    int ch;
    /* Read a character from UART and block until received */
    HAL_UART_Receive(&UART_HANDLE, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
