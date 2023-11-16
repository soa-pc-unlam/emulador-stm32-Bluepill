/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
char serial_ch_msg;
char serial_str_msg[100];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

osThreadId Serial_Write_1Handle;
uint32_t Serial_Write_1Buffer[ 512 ];
osStaticThreadDef_t Serial_Write_1ControlBlock;
osThreadId LedFlashHandle;
uint32_t LedFlashBuffer[ 512 ];
osStaticThreadDef_t LedFlashControlBlock;
osThreadId Serial_Write_2Handle;
uint32_t Serial_Write_2Buffer[ 512 ];
osStaticThreadDef_t Serial_Write_2ControlBlock;
osThreadId Serial_Xmit_StrHandle;
uint32_t Serial_Xmit_StrBuffer[ 512 ];
osStaticThreadDef_t Serial_Xmit_StrControlBlock;
osThreadId Serial_Read_WriHandle;
uint32_t Serial_Read_WriBuffer[ 512 ];
osStaticThreadDef_t Serial_Read_WriControlBlock;
osMessageQId serial_str_queueHandle;
uint8_t serial_str_queueBuffer[ 10 * sizeof( char ) ];
osStaticMessageQDef_t serial_str_queueControlBlock;
osMessageQId serial_rx_queueHandle;
uint8_t serial_rx_queueBuffer[ 1 * sizeof( char ) ];
osStaticMessageQDef_t serial_rx_queueControlBlock;
osSemaphoreId serial_tx_wait_semHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void queue_str_task1(void const * argument);
void led_flash_task(void const * argument);
void queue_str_task2(void const * argument);
void rs232_xmit_msg_task(void const * argument);
void serial_readwrite_task(void const * argument);

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of serial_tx_wait_sem */
  osSemaphoreDef(serial_tx_wait_sem);
  serial_tx_wait_semHandle = osSemaphoreCreate(osSemaphore(serial_tx_wait_sem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of serial_str_queue */
  osMessageQStaticDef(serial_str_queue, 10, char, serial_str_queueBuffer, &serial_str_queueControlBlock);
  serial_str_queueHandle = osMessageCreate(osMessageQ(serial_str_queue), NULL);

  /* definition and creation of serial_rx_queue */
  osMessageQStaticDef(serial_rx_queue, 1, char, serial_rx_queueBuffer, &serial_rx_queueControlBlock);
  serial_rx_queueHandle = osMessageCreate(osMessageQ(serial_rx_queue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Serial_Write_1 */
  osThreadStaticDef(Serial_Write_1, queue_str_task1, osPriorityNormal, 0, 512, Serial_Write_1Buffer, &Serial_Write_1ControlBlock);
  Serial_Write_1Handle = osThreadCreate(osThread(Serial_Write_1), NULL);

  /* definition and creation of LedFlash */
  osThreadStaticDef(LedFlash, led_flash_task, osPriorityLow, 0, 512, LedFlashBuffer, &LedFlashControlBlock);
  LedFlashHandle = osThreadCreate(osThread(LedFlash), NULL);

  /* definition and creation of Serial_Write_2 */
  osThreadStaticDef(Serial_Write_2, queue_str_task2, osPriorityLow, 0, 512, Serial_Write_2Buffer, &Serial_Write_2ControlBlock);
  Serial_Write_2Handle = osThreadCreate(osThread(Serial_Write_2), NULL);

  /* definition and creation of Serial_Xmit_Str */
  osThreadStaticDef(Serial_Xmit_Str, rs232_xmit_msg_task, osPriorityLow, 0, 512, Serial_Xmit_StrBuffer, &Serial_Xmit_StrControlBlock);
  Serial_Xmit_StrHandle = osThreadCreate(osThread(Serial_Xmit_Str), NULL);

  /* definition and creation of Serial_Read_Wri */
  osThreadStaticDef(Serial_Read_Wri, serial_readwrite_task, osPriorityLow, 0, 512, Serial_Read_WriBuffer, &Serial_Read_WriControlBlock);
  Serial_Read_WriHandle = osThreadCreate(osThread(Serial_Read_Wri), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_UART_Receive_IT(&huart1, &serial_ch_msg, sizeof(serial_ch_msg));

  while (1)
  {
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
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

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	static signed portBASE_TYPE xHigherPriorityTaskWoken;

    xSemaphoreGiveFromISR(serial_tx_wait_semHandle, &xHigherPriorityTaskWoken);

    if(xHigherPriorityTaskWoken)
    {
        taskYIELD();
    }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	char rx_msg;

	/* Queue the received byte. */
	if(!xQueueSendToBackFromISR(serial_rx_queueHandle, &rx_msg, &xHigherPriorityTaskWoken))
	{
		/* If there was an error queueing the received byte, freeze. */
		while(1);
	}

	if(xHigherPriorityTaskWoken)
	{
	      taskYIELD();
	}

}

void send_byte_rtos(char ch)
{
    /* Wait until the RS232 port can receive another byte (this semaphore is
     * "given" by the RS232 port interrupt when the buffer has room for another
     * byte.
     */
    while(!xSemaphoreTake(serial_tx_wait_semHandle, portMAX_DELAY));

    /* Send the byte and enable the transmit interrupt (it is disabled by the
     * interrupt).
     */
    HAL_UART_Transmit_IT(&huart1,&ch,sizeof(ch));
}

char receive_byte_rtos(void)
{
    char msg;

    /* Wait for a byte to be queued by the receive interrupt handler. */
    while(!xQueueReceive(serial_rx_queueHandle, &serial_ch_msg, portMAX_DELAY));

    return msg;
}



/* Repeatedly queues a string to be sent to the RS232.
 *   delay - the time to wait between sending messages.  A delay of 1 means
 *           wait 1/100th of a second.
 */
void queue_str_task(const char str[], int delay)
{

    /* Prepare the message to be queued. */


    while(1)
    {
        /* Post the message.  Keep on trying until it is successful. */
        while(!xQueueSendToBack(serial_str_queueHandle, str, portMAX_DELAY));

        /* Wait. */
        vTaskDelay(delay);
    }

}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_queue_str_task1 */
/**
  * @brief  Function implementing the Serial_Write_1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_queue_str_task1 */
void queue_str_task1(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
   queue_str_task("Hola soy Tarea 1, probando ejecucion con FreeRTOS\n", 200);
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_led_flash_task */
/**
* @brief Function implementing the LedFlash thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_led_flash_task */
void led_flash_task(void const * argument)
{
  /* USER CODE BEGIN led_flash_task */
  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    osDelay(1000);
  }
  /* USER CODE END led_flash_task */
}

/* USER CODE BEGIN Header_queue_str_task2 */
/**
* @brief Function implementing the Serial_Write_2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_queue_str_task2 */
void queue_str_task2(void const * argument)
{
  /* USER CODE BEGIN queue_str_task2 */
  /* Infinite loop */
  queue_str_task("Hola soy Tarea 2, probando ejecucion con FreeRTOS\n", 50);
  /* USER CODE END queue_str_task2 */
}

/* USER CODE BEGIN Header_rs232_xmit_msg_task */
/**
* @brief Function implementing the Serial_Xmit_Str thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_rs232_xmit_msg_task */
void rs232_xmit_msg_task(void const * argument)
{
  /* USER CODE BEGIN rs232_xmit_msg_task */
  /* Infinite loop */
    //serial_str_msg msg;
    int curr_char;

    while(1) {
        /* Read from the queue.  Keep trying until a message is received.  This
         * will block for a period of time (specified by portMAX_DELAY). */
        while(!xQueueReceive(serial_str_queueHandle, serial_str_msg, portMAX_DELAY));

        /* Write each character of the message to the RS232 port. */
        curr_char = 0;
        while(serial_str_msg[curr_char] != '\0') {
            send_byte_rtos(serial_str_msg[curr_char]);
            curr_char++;
        }
    }

  /* USER CODE END rs232_xmit_msg_task */
}

/* USER CODE BEGIN Header_serial_readwrite_task */
/**
* @brief Function implementing the Serial_Read_Wri thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_serial_readwrite_task */
void serial_readwrite_task(void const * argument)
{
  /* USER CODE BEGIN serial_readwrite_task */
  /* Infinite loop */

    //serial_str_msg msg;
    char ch;
    int curr_char;
    int done;

    /* Prepare the response message to be queued. */
    strcpy(serial_str_msg, "RV: ");

    while(1) {
        curr_char = 4;
        done = 0;
        do {
            /* Receive a byte from the RS232 port (this call will block). */
            ch = receive_byte_rtos();

            /* If the byte is an end-of-line type character, then finish the
             * string and indicate we are done.
             */
            if((ch == '\r') || (ch == '\n')) {
                serial_str_msg[curr_char] = '\n';
                serial_str_msg[curr_char+1] = '\0';
                done = -1;
            /* Otherwise, add the character to the response string. */
            } else {
            	serial_str_msg[curr_char++] = ch;
            }
        } while(!done);

        /* Once we are done building the response string, queue the response to
         * be sent to the RS232 port.
         */
        while(!xQueueSendToBack(serial_str_queueHandle, serial_str_msg, portMAX_DELAY));
    }

  /* USER CODE END serial_readwrite_task */
}

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
