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
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
//void UartRxTask(void);
//void UartPrintf(const char *format, ...);
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
	int result;
	uint8_t event[EVT_QWIDTH];
	// uint8_t payload[MAX_PAYLOAD];
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
  MX_USB_DEVICE_Init();
    /* USER CODE BEGIN 2 */
    result = 0;

	// initialize the pushbutton handler with mask byte.
	PushButton_Init(0x01);
	// start a timer routine: 10msec period, perpetual
	//result = UsrTimer_Set(10, 0, UartRxTask);
	// UART output test
	UartPrintf("\r\n\t\tSystem Started: %d\r\n", result);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		// check event queue
		if (Evt_DeQueue(event)) {
			switch (event[0]) {
			// pushbutton event ================================================
			// event[1]: button id
			// event[2]: PBTN_SCLK, _DCLK, _TCLK, _LCLK, _DOWN, _ENDN
			case EVT_PBTN_INPUT:

				if (event[2] == PBTN_SCLK) {
					UartPrintf("\r\nButton %d: single click.", event[1]);
				} else if (event[2] == PBTN_LCLK) {
					UartPrintf("\r\nButton %d: long click.", event[1]);
				} else if (event[2] == PBTN_DCLK) {
					UartPrintf("\r\nButton %d: double click.", event[1]);
				} else if (event[2] == PBTN_TCLK) {
					UartPrintf("\r\nButton %d: triple click.", event[1]);

					PushButton_SetMode(PUSHBTN_MODE_UDOWN, true);
					UartPrintf("\r\n --> Switch to up-down mode.");
				} else if (event[2] == PBTN_DOWN) {
					UartPrintf("\r\nButton %d: is being pressed.", event[1]);
				} else if (event[2] == PBTN_ENDN) {
					UartPrintf("\r\nButton %d: has been released.", event[1]);
					PushButton_SetMode(PUSHBTN_MODE_CLICK, true);
					UartPrintf("\r\n --> Switch to click mode.");
				}
				break;


			default:
				break;
			}
		} else {
			// delay here is recommended not to call Evt_DeQueue too frequently
			HAL_Delay(10);
		}
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : TEST_BTN_Pin */
  GPIO_InitStruct.Pin = TEST_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TEST_BTN_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/** Printf() style UART output. Check the standard c library that comes
 * with the compiler for the scope of the format string it supports.
 */
void UartPrintf(const char *format, ...)
{
	char buffer[128];
	uint16_t size;
	va_list args;

	va_start(args, format);
	size = vsprintf(buffer, format, args);
	va_end(args);
	CDC_Transmit_FS((uint8_t*)buffer, size);
	//HAL_UART_Transmit(&huart1, (uint8_t*)buffer, size, 1000);
}

/** This function returns the state (click/release) of the pushbuttons
 * in a uint8_t variable, each bit of which corresponds to a button.
 * Note that 1 implies the button is clicked (pressed) and 0 implies the
 * button is released.
 *
 * \return	pushbutton state packed in a uint8_t.
 */
uint8_t PushButton_Read()
{
	// button released
	if(HAL_GPIO_ReadPin(TEST_BTN_GPIO_Port, TEST_BTN_Pin))
	{
		return 0x00;
	}
	// button pressed
	else
	{
		return 0x01;
	}
}

/** SysTick callback function override.
 */
void SysTick_Handler_1ms()
{
	// UsrTimer_Routine will have 1msec resolution
	UsrTimer_Routine();
}

void SysTick_Handler_5ms()
{
	// blink the LED
	static int counter_5ms = 0;
	if (counter_5ms++ > 200) {
		counter_5ms = 0;
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	}
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
	while (1) {
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
