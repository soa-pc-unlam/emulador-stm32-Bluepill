/*
 * serial.c
 *
 *  Created on: Nov 20, 2023
 *      Author: User
 */

#include <serial.h>

int UartReady=RESET;
int nGlobalRead=0;
char buffer_aux_tx_uart1[BUFFER_SIZE]={0};
char buffer_aux_tx_uart2[BUFFER_SIZE]={0};
char buffer_aux[BUFFER_SIZE]={0};

uint8_t mode_tx_usart= POLLING;


int uart_write_mode_poll(UART_HandleTypeDef *huart, char *buf, int len)
{
    if(HAL_UART_Transmit(huart, (uint8_t*)buf,(uint16_t)len, TIMEOUT_POLL)==HAL_OK)
    {
    	return len;
    }
    else
    {
    	return 0;
    }
}


/**
  * Funcion que segun la configuración, determina el mecanismo por donde se van a enviar los datos
  * al puerto serie. Este puede ser por polling, Interrupciones o por DMA.
  * @param u: estrucutura del huart donde se enviaran los datos por la terminal serial
  * @param buf: String que se va a enviar a la huart
  * @param len: tamaño del string a enviar
  * @return retorna la cantidad de bytes enviados al huart
  */
int uart_write(UART_HandleTypeDef *huart, char *buf, int len)
{
	int nwritten=0;

	switch(mode_tx_usart)
	{
		case POLLING:
			nwritten= uart_write_mode_poll(huart,buf,len);
			break;
		case IRQ:
			//nwritten= uart_write_mode_irq(huart,buf,len);
			break;
		default:
			//Error_Handler();
	}
	return nwritten;
}


/**
  * Funcion que realiza lo mismo qe el printf, pero imprime un mensaje en la terminal serial.
  * Al igual que printf puede recibir cualquier cantidad de parametros
  * @param u: estrucutura del huart donde se enviaran los datos por la terminal serial
  * @param fmt: String que se va a enviar a la huart
  * @return retorna la cantidad de bytes enviados al huart
  */
uint8_t send_uart(UART_HandleTypeDef *huart, const char *fmt, ...) // custom printf() function
{

	char msg[SIZE_MSG_UART_TO_SEND]={0};
	uint8_t result=0;

	va_list argp;
    va_start(argp, fmt);

    //con vsnprintf se arma el msg sin peligro de producir oveflow
    //debido a que se la cadena orignr es mas grande que el destino
    //trunca al mensaje.
    result=vsnprintf(msg, sizeof(msg),fmt,argp);
    uart_write(huart,msg,strlen(msg));
    va_end(argp);

    return result;
}
