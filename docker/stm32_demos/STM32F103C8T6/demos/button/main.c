#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"
#include "stm32_f103.h"
#include "profiling/profil.h"
#include "profiling/gmon.h" /* for _mcleanup() */
#include <unistd.h> /* for _exit() */

int main(void)
{
    int last_button_state, new_button_state;

    init_led();
    init_button();

  last_button_state= GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);

   while(1)
   {

   	new_button_state =GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
       if(new_button_state ^ last_button_state)
       {
           if(new_button_state)
           {
				 //GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
        	   //_exit(0);

           }else
           {
        	   GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
           }
       }
       last_button_state = new_button_state;
   }
}

void _exit(int status)
{
  (void)status; /* not used */
  _mcleanup(); /* write gmon.out file */
  /* turn on all LED's ==> WHITE */
  GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
  for(;;){} /* does not return */
}

