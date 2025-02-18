/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t rData[6];
uint8_t tData[] = "#0001:0002#\n\r";
uint8_t cmdOn[] = "OOOOOO\n\r";
uint8_t cmdOff[] = "XXXXXX\n\r";
uint8_t endline[] = "\n\r";
uint8_t a = 0;
uint8_t a_str[10];
int loaded = 0;

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void SwitchOn(GPIO_TypeDef* LedPort, uint16_t LedPin)
{
  HAL_GPIO_WritePin(LedPort,LedPin,1);
}

void SwitchOff(GPIO_TypeDef* LedPort, uint16_t LedPin)
{
  HAL_GPIO_WritePin(LedPort,LedPin,0);
}

void PowerOn()
{
  SwitchOn(LED1_GPIO_Port, LED1_Pin);
}

void PowerOff()
{
  SwitchOff(LED1_GPIO_Port, LED1_Pin);
}

void sendData(uint8_t* message)
{
  HAL_UART_Transmit(&huart1, message, sizeof(message), 1000);
  while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
}

int DetectorCMD(GPIO_TypeDef* KeyPort, uint16_t KeyPin)
{
  if(HAL_GPIO_ReadPin(KeyPort,KeyPin)){
    return 1;
  }
  else return 0;
}

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    while(!loaded){
      //发送地址
      HAL_UART_Transmit(&huart1, tData, sizeof(tData), 1000);
      while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);

      //检测总控是否识别
      HAL_UART_Receive(&huart1, rData, sizeof(rData), 1000);
      while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_RX);
      if(rData[1] == 'R'){
        loaded = 1;
        PowerOff();
        break;
      }
    }

    //发送指令
    if(DetectorCMD(KEY1_GPIO_Port, KEY1_Pin)){
      for(int i = 10; i>=0; i--){
        HAL_UART_Transmit(&huart1, cmdOn, sizeof(cmdOn), 1000);
        while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
        SwitchOn(LED1_GPIO_Port, LED1_Pin);
      }
    }
    else{

      HAL_UART_Transmit(&huart1, cmdOff, sizeof(cmdOff), 1000);
      while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);

      SwitchOff(LED1_GPIO_Port, LED1_Pin);
    }
    HAL_Delay(1000);
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
