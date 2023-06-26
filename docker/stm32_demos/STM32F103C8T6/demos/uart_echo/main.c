#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"

int main(void)
{
    uint8_t b;

    init_led();

    init_rs232();
    USART_Cmd(USART1, ENABLE);

    while(1) {
        /* Loop until the USART2 has received a byte. */
        while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

        /* Capture the received byte and print it out. */
        b = (USART_ReceiveData(USART1) & 0x7F);
        send_byte('G');
        send_byte('o');
        send_byte('t');
        send_byte(':');
        send_byte(b);
	send_byte('\r');
	send_byte('\n');
    }
return 0;    
}	
