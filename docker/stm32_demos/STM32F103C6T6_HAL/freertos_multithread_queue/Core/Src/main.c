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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
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
volatile xQueueHandle serial_str_queue = NULL;
volatile xSemaphoreHandle serial_tx_wait_sem = NULL;
volatile xQueueHandle serial_rx_queue = NULL;

/* Queue structure used for passing messages. */
typedef struct
{
    char str[100];
} serial_str_msg;

/* Queue structure used for passing characters. */
typedef struct
{
    char ch;
} serial_ch_msg;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void send_byte_rtos(char ch);
char receive_byte_rtos(void);
void led_flash_task( void *pvParameters );
void rs232_xmit_msg_task( void *pvParameters );
void queue_str_task(const char *str, int delay);
void queue_str_task1( void *pvParameters );
void queue_str_task2( void *pvParameters );
void serial_readwrite_task( void *pvParameters );

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
	serial_ch_msg rx_msg;
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
  HAL_UART_Receive_IT(&huart1,(void *)&(rx_msg.ch), sizeof(rx_msg.ch));

  /* Create the queue to hold messages to be written to the RS232. */
  serial_str_queue = xQueueCreate( 10, sizeof( serial_str_msg ) );
  vSemaphoreCreateBinary(serial_tx_wait_sem);
  serial_rx_queue = xQueueCreate( 1, sizeof( serial_ch_msg ) );

  /* Create a task to flash the LED. */
  xTaskCreate( led_flash_task,"LED Flash", 256, NULL, tskIDLE_PRIORITY + 5, NULL );

  /* Create tasks to queue a string to be written to the RS232 port. */
  xTaskCreate( queue_str_task1, "Serial Write 1", 256 , NULL, tskIDLE_PRIORITY + 10, NULL );
  xTaskCreate( queue_str_task2, "Serial Write 2", 256 , NULL, tskIDLE_PRIORITY + 10, NULL );

  /* Create a task to write messages from the queue to the RS232 port. */
  xTaskCreate(rs232_xmit_msg_task, "Serial Xmit Str", 256 , NULL, tskIDLE_PRIORITY + 2, NULL );

  /* Create a task to receive characters from the RS232 port and echo them back to the RS232 port. */
  xTaskCreate(serial_readwrite_task, "Serial Read/Write", 256 ,NULL, tskIDLE_PRIORITY + 10, NULL );

  /* Start running the tasks. */
  vTaskStartScheduler();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


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

/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
   static signed portBASE_TYPE xHigherPriorityTaskWoken;
   //serial_ch_msg rx_msg;

   xSemaphoreGiveFromISR(serial_tx_wait_sem, &xHigherPriorityTaskWoken);

   if(xHigherPriorityTaskWoken)
   {
	 taskYIELD();
   }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
   static signed portBASE_TYPE xHigherPriorityTaskWoken;
   serial_ch_msg rx_msg;

   HAL_UART_Receive_IT(huart,(void *)&(rx_msg.ch), sizeof(rx_msg.ch));


   if(!xQueueSendToBackFromISR(serial_rx_queue, &rx_msg, &xHigherPriorityTaskWoken))
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
	while(!xSemaphoreTake(serial_tx_wait_sem,100));
    //while(!xSemaphoreTake(serial_tx_wait_sem, HAL_MAX_DELAY));

    /* Send the byte and enable the transmit interrupt (it is disabled by the
     * interrupt).
     */
	HAL_UART_Transmit_IT(&huart1,(uint8_t *)&ch, sizeof(ch));
 }

char receive_byte_rtos(void)
{
    serial_ch_msg msg;

    /* Wait for a byte to be queued by the receive interrupt handler. */
    while(!xQueueReceive(serial_rx_queue, &msg, HAL_MAX_DELAY));

    return msg.ch;
}

void led_flash_task( void *pvParameters )
{
    while(1)
    {
        /* Toggle the LED. */

    	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		/* Wait one second. */
		vTaskDelay(1000);
    }
}

void rs232_xmit_msg_task( void *pvParameters )
{
    serial_str_msg msg;
    int curr_char;

    while(1) {
        /* Read from the queue.  Keep trying until a message is received.  This
         * will block for a period of time (specified by portMAX_DELAY). */
        while(!xQueueReceive(serial_str_queue, &msg, portMAX_DELAY));

        /* Write each character of the message to the RS232 port. */
        curr_char = 0;
        while(msg.str[curr_char] != '\0') {
            send_byte_rtos(msg.str[curr_char]);
            curr_char++;
        }
    }
}


/* Repeatedly queues a string to be sent to the RS232.
 *   delay - the time to wait between sending messages.  A delay of 1 means
 *           wait 1/100th of a second.
 */
void queue_str_task(const char *str, int delay)
{
    serial_str_msg msg;

    /* Prepare the message to be queued. */
    strcpy(msg.str, str);

    while(1) {
        /* Post the message.  Keep on trying until it is successful. */
        while(!xQueueSendToBack(serial_str_queue, &msg, portMAX_DELAY));

        /* Wait. */
        vTaskDelay(delay);
    }
}

void queue_str_task1( void *pvParameters )
{
    queue_str_task("Hola soy Tarea 1, probando ejecucion con FreeRTOS\r\n", 5000);
}

void queue_str_task2( void *pvParameters )
{
    queue_str_task("Hola soy Tarea 2, probando ejecucion con FreeRTOS\r\n", 5000);
}

void serial_readwrite_task( void *pvParameters )
{
    serial_str_msg msg;
    char ch;
    int curr_char;
    int done;

    /* Prepare the response message to be queued. */
    strcpy(msg.str, "RV: ");

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
                msg.str[curr_char] = '\n';
                msg.str[curr_char+1] = '\0';
                done = -1;
            /* Otherwise, add the character to the response string. */
            } else {
                msg.str[curr_char++] = ch;
            }
        } while(!done);

        /* Once we are done building the response string, queue the response to
         * be sent to the RS232 port.
         */
        while(!xQueueSendToBack(serial_str_queue, &msg, portMAX_DELAY));
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
