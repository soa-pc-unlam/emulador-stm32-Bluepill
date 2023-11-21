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
#include "tim.h"
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
// Habilitacion de debug para la impresion por el puerto serial ...
//----------------------------------------------

#define STATE_INIT        1
#define STATE_FREE        2
#define STATE_PUSH        3

#define EVENT_CONTINUE    1
#define EVENT_LED_ON      2
#define EVENT_LED_OFF     3
#define EVENT_BUTTON_FREE 4
#define EVENT_BUTTON_PUSH 5
#define EVENT_TIMEOUT     6

#define MAX_CANT_SENSORS 2
#define ACTUATOR_LED      0
#define SENSOR_BUTTON     1

#define LED_ON  '1'
#define LED_OFF '2'

#define FREE 0
#define PUSH 1

#define FALSE 0
#define TRUE  1

#define TIME_TIMEOUT 		  5
#define TIMEOUT_NOT_FULFILLED 1
#define TIMEOUT_COMPLETED 	  2

#define SERIAL_DEBUG_ENABLED 1

#if SERIAL_DEBUG_ENABLED
  #define DebugPrint(str)\
      {\
        send_uart(&huart1,"%s\r\n",str);\
      }
#else
  #define DebugPrint(str)
#endif

#define DebugPrintEstado(estado,evento)\
      {\
        DebugPrint("----------------------------");\
        DebugPrint(estado);\
        DebugPrint(evento);\
        DebugPrint("----------------------------");\
}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


//----------------------------------------------
struct stSensor
{
  int  gpio_pin;
  GPIO_TypeDef*  gpio_char;
  int  state;
  long current_value;
  long previous_value;
};

struct stSensor sensors[MAX_CANT_SENSORS];

//----------------------------------------------

//----------------------------------------------
struct stEvento
{
  int type;
  int param1;
  int param2;
};

struct stEvento event;


int state;
int timeout;

int state_timeout=TIMEOUT_NOT_FULFILLED;

void sleep()
{
   int i=1000000;
   while(i-->0);
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void led_off()
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,RESET);
}

void led_on()
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,SET);
	//HAL_GPIO_WritePin(GPIOC,sensors[ACTUATOR_LED].gpio_pin, SET);
}

int verify_state_button()
{
	sensors[SENSOR_BUTTON].current_value=HAL_GPIO_ReadPin(GPIOA,sensors[SENSOR_BUTTON].gpio_pin);


    int current_value =sensors[SENSOR_BUTTON].current_value;

    //myprintf(" ************************actual_boton:%d  prevoi_boton:%d\n",current_value,sensors[SENSOR_BUTTON].previous_value);

    int previous_value =sensors[SENSOR_BUTTON].previous_value;



    if(current_value!=previous_value)
    {
        //myprintf(" ************************actual_boton:%d  prevoi_boton:%d\n",current_value,sensors[SENSOR_BUTTON].previous_value);
        sensors[SENSOR_BUTTON].previous_value= current_value;

        if(current_value==PUSH)
        {
            //myprintf("\n*******************EVENT PUSH************\n");
            event.type=EVENT_BUTTON_PUSH;

        }
        return TRUE;
    }

    return FALSE;
}

int verify_state_bluetooth()
{
    uint8_t  receive_data=0;


	/* Capture the received byte and print it out. */
    HAL_UART_Receive(&huart1, &receive_data, sizeof(receive_data), 10);

	if(receive_data==LED_ON)
	{
		//myprintf("\n*******************PRENDE LED************\n");
		event.type=EVENT_LED_ON;
		return TRUE;
	}
	else if(receive_data==LED_OFF)
	{
		//myprintf("\n*******************APAGA LED************\n");
		event.type=EVENT_LED_OFF;
		return TRUE;
	}



    return FALSE;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	 if(htim->Instance==TIM2)
	 {
		 state_timeout=TIMEOUT_COMPLETED;
	 }
}

int verify_timeout()
{
    if(state_timeout==TIMEOUT_COMPLETED)
    {
        event.type=EVENT_TIMEOUT;
        state_timeout=TIMEOUT_NOT_FULFILLED;
        return TRUE;
    }

    return FALSE;
}

void get_event()
{
    if((verify_state_button()==TRUE || verify_state_bluetooth()==TRUE || verify_timeout()==TRUE))
    {
        return;
    }
    event.type=EVENT_CONTINUE;
}



void state_machine()
{
    get_event();

    switch(state)
    {
        case (STATE_INIT):
        {

            switch ( event.type)
            {
                case(EVENT_CONTINUE):
                {
                 //   DebugPrintEstado("INIT","CONTINUE");
                    led_off();
                    state=STATE_FREE;
                    break;
                }
                default:
               //     myprintf("Error\r\n");
                    break;
            }

        }
        break;
        case (STATE_FREE):
        {

            switch (event.type)
            {

                case(EVENT_LED_OFF):
                {
                    DebugPrintEstado("FREE","LED_OFF");
                    led_off();
                    break;
                }
                case(EVENT_LED_ON):
                {
                    DebugPrintEstado("FREE","LED_ON");
                    led_on();
                    break;
                }
                case(EVENT_BUTTON_PUSH):
                {
                    DebugPrintEstado("FREE","PUSH_BUTTON");

                    send_uart(&huart1,"Boton Presionado\r\n");
               //     send_byte('R');

                    //__HAL_TIM_SET_COUNTER(htim2, 0);
                    //HAL_TIM_Base_Start_IT(&htim2);

                    //HAL_TIM_Base_Stop_IT(&htim2);
                    //__HAL_TIM_SET_COUNTER(&htim2, 0);
                   // __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
                    HAL_TIM_Base_Start_IT(&htim2);

                    state=STATE_PUSH;
                    break;
                }
                case(EVENT_CONTINUE):
                {
                   // DebugPrintEstado("FREE","CONTINUE");
                    break;
                }
                default:
             //       myprintf("Error\r\n");
                    break;
            }

        }
        break;
        case (STATE_PUSH):
        {
            switch (event.type)
            {

                case(EVENT_LED_OFF):
                {
                    DebugPrintEstado("PUSH","LED_OFF");
                    led_off();
                    break;
                }
                case(EVENT_LED_ON):
                {
                    DebugPrintEstado("PUSH","LED_ON");
                    led_on();
                    break;
                }
                case(EVENT_BUTTON_PUSH):
                {
                    DebugPrintEstado("BUTTON_PUSH","EVENT_BUTTON_PUSH");
                    break;
                }
                case(EVENT_BUTTON_FREE):
                case(EVENT_TIMEOUT):
                {
                    DebugPrintEstado("BUTTON_PUSH","FREE");

                    send_uart(&huart1,"Boton Liberado\r\n");
                    //send_byte('F');

                    HAL_TIM_Base_Stop_IT(&htim2);

                    state=STATE_FREE;

                    break;
                }
                case(EVENT_CONTINUE):
                {
                    //DebugPrintEstado("PUSH","CONTINUE");
                    break;
                }
                default:
                    //myprintf("Error\r\n");
                    break;
            }

        }
        break;
    }

    event.type=EVENT_CONTINUE;
}


void do_init()
{

    state= STATE_INIT;

    sensors[ACTUATOR_LED].gpio_pin=(int)GPIO_PIN_13;
    sensors[ACTUATOR_LED].gpio_char=GPIOC;
    sensors[SENSOR_BUTTON].gpio_pin=(int)GPIO_PIN_0;
    sensors[SENSOR_BUTTON].gpio_char=GPIOA;

    timeout=FALSE;
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */



  do_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  state_machine();
	  //sleep();
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
