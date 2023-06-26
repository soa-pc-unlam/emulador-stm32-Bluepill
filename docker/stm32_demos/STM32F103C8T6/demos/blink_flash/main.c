#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"
#include "stm32_f103.h"

void busyLoop(uint32_t delay )
{
  while(delay) delay--;
}

int main(void)
{
    init_led();

    while(1) {
       GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
       busyLoop(5000000);
       GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
       busyLoop(5000000);
    }
}
