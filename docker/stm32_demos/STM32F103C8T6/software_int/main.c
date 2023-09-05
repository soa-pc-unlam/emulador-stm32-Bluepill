#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"

int led_status=0;

void myDelay(unsigned long delay )
{
    while(delay) delay--;
}

void EXTI0_IRQHandler(void)
{
    /* Make sure the line has a pending interrupt
     * (should this always be true if we are inside this interrupt handle? */

    if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
        /* Toggle LED */
        if(led_status==0)
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
			led_status=1;
		}
		else
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
			led_status=0;
		}

        /* Clear the pending interrupt flag that triggered this interrupt. */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

int main(void)
{
    init_led();

    init_button();
    enable_button_interrupts();

    while(1) {
          /* Generate a software interrupt on EXTI line 0 */
          EXTI_GenerateSWInterrupt(EXTI_Line0);

          myDelay(500000);
    }
}


