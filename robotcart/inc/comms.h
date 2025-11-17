#ifndef __COMMS_H__
#define __COMMS_H__

#include "main.h"

// Initialize UART
void uart_init(void);
void uart_send_char(char c);
char uart_receive_char(void);
void uart_handle_command(char cmd);

#endif
