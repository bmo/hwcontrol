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

void cmd_proc(uint8_t *buffer, uint16_t size) {
	// handle commands
	uint8_t *cp = buffer;
	uint16_t i = size;

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
	PushButton_Init(0x01);

	// start a timer routine: 10msec period, perpetual
	//result = UsrTimer_Set(10, 0, UartRxTask);
	// UART output test
	report_version();
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
				//UartPrintf("\r\nEncoder UP %d: delta %d", event[1], event[2]);
				if (event[2] == 1) {
					UartPrintf("U;");
				} else {
					UartPrintf("U%02X;", event[2]);
				}
				break;

			case EVT_ENCODER_DOWN:
				// event[1]: encoder_id
				// event[2]: encoder_delta
				//UartPrintf("\r\nEncoder DOWN %d: delta %d", event[1], event[2]);
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

				if (event[2] == PBTN_SCLK) { // single click event
					if (event[1] == 0) {
						UartPrintf("S;", event[1]);
					} else {
						UartPrintf("X%1dS;", event[1]);
					}
				} else if (event[2] == PBTN_LCLK) { // long click event
					if (event[1] == 0) {
						UartPrintf("L;", event[1]);
					} else {
						UartPrintf("X%1dL;", event[1]);
					}
				} else if (event[2] == PBTN_DCLK) { // double click
					if (event[1] == 0) {
						UartPrintf("C;", event[1]);
					} else {
						UartPrintf("X%1dC;", event[1]);
					}
				} else if (event[2] == PBTN_TCLK) {
					//UartPrintf("\r\nButton %d: triple click.", event[1]);
					//PushButton_SetMode(PUSHBTN_MODE_UDOWN, true);
					//UartPrintf("\r\n --> Switch to up-down mode.");
				} else if (event[2] == PBTN_DOWN) {
					//UartPrintf("\r\nButton %d: is being pressed.", event[1]);
				} else if (event[2] == PBTN_ENDN) {
					//UartPrintf("\r\nButton %d: has been released.", event[1]);
					//PushButton_SetMode(PUSHBTN_MODE_CLICK, true);
					//UartPrintf("\r\n --> Switch to click mode.");
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

  /*Configure GPIO pins : TEST_BTN_Pin ENC_CLK_Pin ENC_DATA_Pin */
  GPIO_InitStruct.Pin = TEST_BTN_Pin|ENC_CLK_Pin|ENC_DATA_Pin;
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
	va_list args;

	va_start(args, format);
	size = vsprintf(buffer, format, args);
	va_end(args);
	CDC_Transmit_FS((uint8_t*)buffer, size);
	//HAL_UART_Transmit(&huart1, (uint8_t*)buffer, size, 1000);
}

void Encoder_Init()
{
   // register pushbutton main routine
   //	UsrTimer_Set(PUSHBTN_TMR_PERIOD, 0, PushButton_Routine);
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
// test a KY-040 style rotary encoder module on a blue pill board
// uncomment either LIBMAPLE_CORE or STM32DUINO_CORE
//#define LIBMAPLE_CORE
//#define STM32DUINO_CORE
// for STM32DUINO_CORE be sure to select a serial port in the tools menu

// define encoder pins
// clock and data are from the silkscreen on the encoder module
#define ENC_CLK PB8
#define ENC_DATA PB9

// call this from the systick interrupt every millisecond
// modified from the original code by dannyf
// at https://github.com/dannyf00/My-MCU-Libraries---2nd-try/blob/master/encoder1.c
int encoder1_read(void)
{
  volatile static uint8_t ABs = 0;
  ABs = (ABs << 2) & 0x0f; //left 2 bits now contain the previous AB key read-out;
  ABs |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) << 1) | HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);
  switch (ABs)
  {
    case 0x0d:
      return +1;
      break;
    case 0x0e:
      return -1;
      break;
  }
  return 0;
}
// generic encoder read with two pins
int encoder_read(GPIO_TypeDef *clk_bank, uint16_t clk_pin, GPIO_TypeDef *data_bank, uint16_t data_pin)
{
  volatile static uint8_t ABs = 0;
  ABs = (ABs << 2) & 0x0f; //left 2 bits now contain the previous AB key read-out;
  ABs |= (HAL_GPIO_ReadPin(clk_bank, clk_pin) << 1) | HAL_GPIO_ReadPin(data_bank, data_pin);
  switch (ABs)
  {
    case 0x0d:
      return +1;
      break;
    case 0x0e:
      return -1;
      break;
  }
  return 0;
}

#define MAX_ENCODER_INTERVAL 100
#define MAX_ENCODER_DIVIDER 25

void encoder1_routine_1ms() {
	static unsigned int msecs_since_last;
	static int duration = 0;
	static int encoder_1_value = 0;
	static int delta = 0;

	uint8_t mult = 0;

	uint8_t event[EVT_QWIDTH];

	// increment duration
	duration--;

	msecs_since_last++;
	if (msecs_since_last > MAX_ENCODER_INTERVAL) msecs_since_last = MAX_ENCODER_INTERVAL;
	mult = msecs_since_last >= MAX_ENCODER_INTERVAL ? 1 : (uint8_t)((MAX_ENCODER_INTERVAL - msecs_since_last)/ MAX_ENCODER_DIVIDER)+1;

	// read encoder values
	delta = encoder1_read();
	if (delta) msecs_since_last = 0;
	encoder_1_value += delta * mult;


	if (duration < 0) {
		if (encoder_1_value < 0) {
			event[0] = EVT_ENCODER_DOWN;
			event[2] = (uint8_t) (-encoder_1_value);
			event[1] = 1;

			// post the event to indicate the end of the down state
			Evt_EnQueue(event);
		}

		if (encoder_1_value > 0) {
			event[0] = EVT_ENCODER_UP;
			event[2] = (uint8_t) (encoder_1_value);
			event[1] = 1;

			// post the event to indicate the end of the down state
			Evt_EnQueue(event);
		}
		duration = reg_r;
		encoder_1_value = 0;
	}

}
/** SysTick callback function override.
 */
void SysTick_Handler_1ms()
{
	int delta = 0;
	int duration = 0;
	int encoder_1_value = 0;
	uint8_t event[EVT_QWIDTH];
	// UsrTimer_Routine will have 1msec resolution
	encoder1_routine_1ms();
	UsrTimer_Routine();
	return;
	delta = encoder1_read();

	if (delta == -1)
	{
		event[0] = EVT_ENCODER_DOWN;
		event[2] = (uint8_t) (1);
		event[1] = 1;

		// post the event to indicate the end of the down state
		Evt_EnQueue(event);
	}

	if (delta == 1) {
		event[0] = EVT_ENCODER_UP;
		event[2] = (uint8_t) (1);
		event[1] = 1;

		// post the event to indicate the end of the down state
		Evt_EnQueue(event);
	}

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
