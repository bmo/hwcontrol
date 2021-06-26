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

uint8_t reg_e = 0x01;
uint8_t reg_r = 0x0a;
uint8_t reg_c = 0x28;
uint8_t reg_l = 0x64;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void encoder1_routine_1ms(void);
/* USER CODE BEGIN PFP */
//void UartRxTask(void);
//void UartPrintf(const char *format, ...);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t uint8_toupper(uint8_t c){
	if (c >= 'a' && c <= 'z') {
		c = c ^ 0x20;
	}
	return c;
}
void report_version() {
	UartPrintf("F0304;");
}


void set_reg_val(uint8_t *reg, uint8_t *str){
	unsigned int value;
	if (isxdigit(*str) && isxdigit(*(str + 1)))
	{
		sscanf((char *)str, "%02X", &value);
		*reg = (uint8_t) (value & 0xff);
	}
}
uint8_t read_config(uint32_t offset){
	uint8_t val = 0;
	offset= offset + 0x0800FC00;
	val = *(__IO uint8_t*) offset;
	return val;
}

void cmd_proc(uint8_t *buffer, uint16_t size) {
	// handle commands
	uint8_t *cp = buffer;
	uint16_t i = size;
	uint16_t j = 0;

	if (size == 0)
		return;

	// uppercase everything
	while (i--) {
		*cp = uint8_toupper(*cp);
		cp++;
	}

	switch (buffer[0]) {
	case 'F':
		if (size != 2)
			return;
		// enqueue F<version>;
		report_version();
		break;
	case 'Z':
		// handle Zr; ZrNN; where r is E,R,C,L, NN is a hex number
		if (size < 3 || size > 5)
			return;  //

		uint8_t is_set = (size == 5);

		switch (buffer[1]) {

		case 'E':
			// enqueue 'ZE<hex>;'
			if (is_set) {
				set_reg_val((uint8_t*) &reg_e, &buffer[2]);
			}
			UartPrintf("ZE%02X;", reg_e);

			break;

		case 'R':
			// enqueue 'ZR<hex>;
			if (is_set) {
				set_reg_val((uint8_t*) &reg_r, &buffer[2]);
			}
			UartPrintf("ZR%02X;", reg_r);
			break;

		case 'C':
			// enqueue 'ZC<hex>;'
			if (is_set) {
				set_reg_val((uint8_t*) &reg_c, &buffer[2]);
			}
			UartPrintf("ZC%02X;", reg_c);
			break;

		case 'L':
			// enqueue 'ZL<hex>;'
			if (is_set) {
				set_reg_val((uint8_t*) &reg_l, &buffer[2]);
			}

			UartPrintf("ZL%02X;", reg_l);

			break;
		}
		break;

	case 'I':
		// Handle Innn; where n is 0,1 corresponding to each of the LEDs
		if (size != 5)
			return;
		if (buffer[1] == '1') {
			HAL_GPIO_WritePin( LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
		}
		if (buffer[2] == '1') {
			HAL_GPIO_WritePin( LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		}
		if (buffer[3] == '1') {
			HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		}
		if (buffer[1] == '0') {
			HAL_GPIO_WritePin( LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
		}
		if (buffer[2] == '0') {
			HAL_GPIO_WritePin( LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		}
		if (buffer[3] == '0') {
			HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
		}
		break;

	case 'C':

		for(j=0; j<256;j++) {
			UartPrintf("%02X;", read_config(j));
		}
		UartPrintf("\n");
		break;

	default:
		break;
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t event[EVT_QWIDTH];
	uint8_t usb_data[USB_BUFFER_SIZE];
	uint8_t single_cmd[CMD_BUFFER_SIZE];
	RingBufferU8 rb1;
	uint8_t saw_terminator=0;
	int len = 0;

	//uint8_t usb_payload[MAX_PAYLOAD];

	RingBufferU8_init(&rb1, usb_data, USB_BUFFER_SIZE);
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

	// initialize the pushbutton handler with mask byte.
	PushButton_Init(0x0F);

	// start a timer routine: 10msec period, perpetual
	// esult = UsrTimer_Set(10, 0, UartRxTask);

	report_version();
	UsrTimer_Set(1,0,encoder1_routine_1ms);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		// check event queue
		if (Evt_DeQueue(event)) {
			switch (event[0]) {

			case EVT_ENCODER_UP:
				// event[1]: encoder_id
				// event[2]: encoder_delta
				if (event[2] == 1) {
					UartPrintf("U;");
				} else {
					UartPrintf("U%02X;", event[2]);
				}
				break;

			case EVT_ENCODER_DOWN:
				// event[1]: encoder_id
				// event[2]: encoder_delta
				if (event[2] == 1) {
					UartPrintf("D;");
				} else {
					UartPrintf("D%02X;",event[2]);
				}
				break;

			// pushbutton event ================================================
			// event[1]: button id
			// event[2]: PBTN_SCLK, _DCLK, _TCLK, _LCLK, _DOWN, _ENDN

			case EVT_PBTN_INPUT:
				//UartPrintf("\r\nButton id %d, event %d\r\n", event[1], event[2]);
				if (event[2] == PBTN_SCLK) { // single click event
					if (event[1] == 0) {
						UartPrintf("S;");
					} else {
						UartPrintf("X%1dS;", event[1]);
					}
					// for testing of CDC_Notify_FS
					// setting 0x02 will set PTT for DTR if Active High
					// CDC_Notify_FS(0x00);
				} else if (event[2] == PBTN_LCLK) { // long click event
					if (event[1] == 0) {
						UartPrintf("L;");
					} else {
						UartPrintf("X%1dL;", event[1]);
					}
					// for testing of CDC_Notify_FS
					// setting 0x02 will clear PTT for DTR if Active High
					// CDC_Notify_FS(0x02);
				} else if (event[2] == PBTN_DCLK) { // double click
					if (event[1] == 0) {
						UartPrintf("C;");
					} else {
						UartPrintf("X%1dC;", event[1]);
					}
				} else if (event[2] == PBTN_TCLK) {
					//UartPrintf("\r\nButton %d: triple click.", event[1]);

				} else if (event[2] == PBTN_DOWN) {
					//UartPrintf("\r\nButton %d: is being pressed.", event[1]);
				} else if (event[2] == PBTN_ENDN) {
					// no button
				}
				break;

			case EVT_USB_DATA:
				// evt[1] = size
				// evt[16]..[16+USB_MAX_PAYLOAD] is data
				saw_terminator = RingBufferU8_write(&rb1, (const unsigned char *)&event[16], event[1], ';');

				// handle the commands
				if (saw_terminator) {
					while ((len = RingBufferU8_readUntil(&rb1, single_cmd, CMD_BUFFER_SIZE, ';'))) {
						single_cmd[len] = 0;
						cmd_proc(single_cmd, len);
						//UartPrintf("USB cmd: [%s]\n",single_cmd);
					}
				}
				break;
			default:
				break;
			}
		} else {
			// delay here is recommended not to call Evt_DeQueue too frequently
			HAL_Delay(5);
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
  HAL_GPIO_WritePin(Blinker_GPIO_Port, Blinker_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED0_Pin|LED1_Pin|LED2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : Blinker_Pin */
  GPIO_InitStruct.Pin = Blinker_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Blinker_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : EncoderButton_Pin Button1_Pin Button2_Pin Button3_Pin */
  GPIO_InitStruct.Pin = EncoderButton_Pin|Button1_Pin|Button2_Pin|Button3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED0_Pin LED1_Pin LED2_Pin */
  GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin|LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_CLK_Pin ENC_DATA_Pin */
  GPIO_InitStruct.Pin = ENC_CLK_Pin|ENC_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/** Printf() style UART output. Check the standard c library that comes
 * with the compiler for the scope of the format string it supports.
 */
void UartPrintf(const char *format, ...)
{
	char buffer[128];
	uint16_t size;
	uint8_t tries = 100;
	va_list args;

	va_start(args, format);
	size = vsprintf(buffer, format, args);
	va_end(args);
	while (USBD_BUSY == CDC_Transmit_FS((uint8_t*)buffer, size) && tries--)
		{
			HAL_Delay(5);
		}
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
	uint8_t buttons = 0;
	// button pushed
	if (!HAL_GPIO_ReadPin(EncoderButton_GPIO_Port, EncoderButton_Pin))
	{
		buttons |= 1;
	}
	if (!HAL_GPIO_ReadPin(Button1_GPIO_Port, Button1_Pin)) {
		buttons |= 2;
	}
	if (!HAL_GPIO_ReadPin(Button2_GPIO_Port, Button2_Pin)) {
		buttons |= 4;
	}
	if (!HAL_GPIO_ReadPin(Button3_GPIO_Port, Button3_Pin)) {
		buttons |= 8;
	}
	//if (buttons) {
	//	UartPrintf("Buttons! %02x",buttons);
	//}

	return buttons;
}


/** SysTick callback function override.
 */
void SysTick_Handler_1ms()
{
	// UsrTimer_Routine will have 1msec resolution
	//encoder1_routine_1ms();
	UsrTimer_Routine();
	return;
}

void SysTick_Handler_5ms()
{
	// blink the LED
	static int counter_5ms = 0;
	if (counter_5ms++ > 200) {
		counter_5ms = 0;
		HAL_GPIO_TogglePin(Blinker_GPIO_Port, Blinker_Pin);
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
