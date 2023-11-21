#include "stm32f1xx_hal.h"

#include <string.h>
#include <stdarg.h>


#define BUFFER_SIZE 255
#define SIZE_MSG_UART_TO_SEND 255
#define TIMEOUT_POLL 1000

#define POLLING 1
#define IRQ 	2




int uart_write_mode_poll(UART_HandleTypeDef *u, char *buf, int len);
int uart_write_mode_irq(UART_HandleTypeDef *u, char *buf, int len);
int uart_write(UART_HandleTypeDef *u, char *buf, int len);
uint8_t send_uart(UART_HandleTypeDef *u, const char *fmt, ...) ;

