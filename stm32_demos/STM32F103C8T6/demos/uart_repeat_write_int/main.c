#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"

int led_status=0;

void USART2_IRQHandler(void)
{
    /* Make sure the interrupt was triggered by a transmit.  This should
     * always be true.
     */
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
        /* Toggle the LED just to show that progress is being made. */
        if (led_status){
        	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
        	led_status=1;
        }
        else{
        	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
        	led_status=0;
        }

        /* Send the next byte */
        USART_SendData(USART1, 'X');
    }
}

int main(void)
{
    init_led();

    init_button();

    init_rs232();
    enable_rs232_interrupts();
    enable_rs232();

    /* Send the first byte and enable the interrupt. */
    USART_SendData(USART1, 'x');
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    /* Infinite loop - when the previous finishes, the interrupt will trigger.
     * The interrupt will send the next byte.
     */
    while(1);
}
