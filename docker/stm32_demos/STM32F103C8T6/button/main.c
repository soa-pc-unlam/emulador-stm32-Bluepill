#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"
#include "stm32_f103.h"
#include "myprintk.h"

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

#define TIME_TIMEOUT 5

// Habilitacion de debug para la impresion por el puerto serial ...
//----------------------------------------------
#define SERIAL_DEBUG_ENABLED 0

#if SERIAL_DEBUG_ENABLED
  #define DebugPrint(str)\
      {\
        myprintf("%s\r\n",str);\
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
//----------------------------------------------
// Note that we need to multiply the period by 2 in order to work properly.
// Not sure why...
uint16_t period_value = (uint16_t) 2 * (36000000 / 65535);

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

void sleep()
{
   int i=1000000;
   while(i-->0);
}

/* Timer functions */
void init_timer(void) {
    /* Configure peripheral clock. */
    /* Let's leave PCLK1 at it's default setting of 36 MHz. */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Time base configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 5 * period_value - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 65535;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // Enable the timer
    TIM_Cmd(TIM2, ENABLE);
}



void led_off()
{
    GPIO_WriteBit(GPIOC,sensors[ACTUATOR_LED].gpio_pin,Bit_RESET);
}

void led_on()
{
    GPIO_WriteBit(GPIOC,sensors[ACTUATOR_LED].gpio_pin,Bit_SET);
}

int verify_state_button()
{
    sensors[SENSOR_BUTTON].current_value=GPIO_ReadInputDataBit(GPIOA,sensors[SENSOR_BUTTON].gpio_pin);

    
    int current_value =sensors[SENSOR_BUTTON].current_value;


    int previous_value =sensors[SENSOR_BUTTON].previous_value;
    
    
    
    if(current_value!=previous_value)
    {

        sensors[SENSOR_BUTTON].previous_value= current_value;

        if(current_value==PUSH)
        {

            event.type=EVENT_BUTTON_PUSH;
            
        }
        return TRUE;
    }

    return FALSE;
}

int verify_state_bluetooth()
{
    uint8_t  receive_data=0;

    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        /* Capture the received byte and print it out. */
        receive_data= (USART_ReceiveData(USART1) & 0x7F);

        if(receive_data==LED_ON)
        {

            event.type=EVENT_LED_ON;
        }
        else if(receive_data==LED_OFF)
        {

            event.type=EVENT_LED_OFF;
        }

        return TRUE;
    }
    return FALSE;
}


int verify_timeout()
{
    uint16_t curr_raw_count = TIM_GetCounter(TIM2);
    uint16_t curr_second_count = (curr_raw_count / period_value) + 1;

    if(curr_second_count==TIME_TIMEOUT)
    {
        event.type=EVENT_TIMEOUT;
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
                    led_off();
                    state=STATE_FREE;
                    break;
                }
                default:            
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

                    myprintf("Boton Presionado\r\n");
            
                    TIM_Cmd(TIM2, ENABLE);
                    
                    state=STATE_PUSH;
                    break;
                }
                case(EVENT_CONTINUE):
                {
               
                    break;
                }
                default:            
            
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

                    myprintf("Boton Liberado\r\n");
            

                    TIM_Cmd(TIM2, DISABLE);
                    
                    state=STATE_FREE;
                    
                    break;
                }
                case(EVENT_CONTINUE):
                {
                    break;
                }
                default:            
                    break;
            }
            
        }
        break;
    }
    
    event.type=EVENT_CONTINUE;
}

void do_init()
{
    init_led();
    init_button();
    init_timer();

    init_rs232();
    USART_Cmd(USART1, ENABLE);
    
    state= STATE_INIT;
    
    sensors[ACTUATOR_LED].gpio_pin=GPIO_Pin_13;
    sensors[ACTUATOR_LED].gpio_char=GPIOC;
    sensors[SENSOR_BUTTON].gpio_pin=GPIO_Pin_0;
    sensors[SENSOR_BUTTON].gpio_char=GPIOA;

    timeout=FALSE;
}

int main(void)
{
   do_init();

   while(1)
   {
        state_machine();
        sleep();
   }
}
