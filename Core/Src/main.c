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
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Lib/w25qxx.h"
#include "Lib/ee24.h"
#include "ht1621b.h"
#include "rbt_delay.h"
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
uint32_t sysclk;
uint32_t pclk1, pclk2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	RBT_Delay_Init();
	/* EEPROM R/W test, based on https://github.com/nimaltd/ee24 */
//	uint8_t testData[6] = {'C', 'H', 'I', 'N', 'G', 'O'};
//	uint8_t data[1024];
//  if (ee24_isConnected())
//  {
//		ee24_write(0, testData, 6, 1000);
//    ee24_read(0, data, 1024, 1000);
//  }
//	
	
	/* SPI FLASH R/W test, based on https://github.com/nimaltd/w25qxx */
	// block: 64k (16 sector), sector = 4kb (16 page), page = 256byte
	// 1Mb = 16 block = 256 sector = 4096 page
	// 8Mb = 128 block = 2048 sector = 32768 page
//	W25qxx_Init();
//	uint8_t wbuffer[6] = {'C', 'H', 'I', 'N', 'G', 'O'};
//	uint8_t rbuffer[6];
//	W25qxx_ReadSector(rbuffer, 1, 0, 6);
//	W25qxx_EraseSector(1);
//	W25qxx_ReadSector(rbuffer, 1, 0, 6);
//	W25qxx_WriteSector(wbuffer, 1, 0, 6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	// HT1621B液晶屏演示
	HT1621B_Init();
	RBT_Delay_MS(10);
	//HT1621B_Scan(0, 11);
	// 清空液晶屏显示
	HT1621B_TurnOff_All();
//	// 显示信号等级（从0到3）
//	QYH04418_Signal(0);
//	HAL_Delay(500);
//	QYH04418_Signal(1);
//	HAL_Delay(500);
//	QYH04418_Signal(2);
//	HAL_Delay(1500);
//	QYH04418_Signal(3);
//	HAL_Delay(1500);
//	// 显示摄氏度（含单位）
//	QYH04418_Celsius(27.3, 4);
//	HAL_Delay(1500);
	// 显示百分比（含百分比符号）
	QYH04418_Percent(-13.5, 4);
	for (int i = 0; i < 1000; i ++) {
		QYH04418_Percent( (float)i / 10 * -1, 3);
		HAL_Delay(100);
	}
	uint8_t pData[256];
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_StatusTypeDef result = HAL_UART_Receive(&huart1, pData, 256, 50);
		uint8_t j;
		if (result == HAL_OK) {
			for (int i = 0; i < 256; i ++) {
				j = pData[i];
			}
		}
		else if (result == HAL_ERROR) {
			j = 0;
		}
		else if (result == HAL_TIMEOUT) {
			j = 1;
		}
			
		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		HAL_Delay(500);
		
		sysclk = HAL_RCC_GetSysClockFreq();
		pclk1 = HAL_RCC_GetPCLK1Freq();
		pclk2 = HAL_RCC_GetPCLK2Freq();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
