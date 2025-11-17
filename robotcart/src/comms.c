#include "comms.h"
#include "motor.h"
#include "lights.h"   // ✅ Add this line if it’s not already there
#include <stdbool.h>


void uart_init(void) {
    // Enable GPIOA and USART1 clocks
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // PA9 = TX (AF1), PA10 = RX (AF1)
    GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
    GPIOA->MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1); // Alternate function
    GPIOA->AFR[1] &= ~((0xF << (4 * (9 - 8))) | (0xF << (4 * (10 - 8)))); // Clear AF bits
    GPIOA->AFR[1] |= (1 << (4 * (9 - 8))) | (1 << (4 * (10 - 8)));        // AF1 for PA9, PA10

    // Configure USART1
    USART1->BRR = 8000000/9600;    // Baud ~38400 for BLE
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;  // Enable TX, RX, USART
}

void uart_send_char(char c) {
    while (!(USART1->ISR & USART_ISR_TXE));  // Wait until transmit data register empty
    USART1->TDR = c;
}

char uart_receive_char(void) {
    while (!(USART1->ISR & USART_ISR_RXNE)); // Wait until data is received
    return (char)(USART1->RDR & 0xFF);
}

void uart_handle_command(char cmd) {

    // Clear previous states
    turning_left = false;
    turning_right = false;
    reversing = false;

    switch (cmd) {
        case 'F':
            forward();
            uart_send_char('F');
            break;

        case 'B':
            backward();
            reversing = true;
            uart_send_char('B');
            break;

        case 'L':
            left();
            turning_right = true;
            uart_send_char('L');
            break;

        case 'R':
            right();
            turning_left = true;
            uart_send_char('R');
            break;

        case 'S':
            stop();
            uart_send_char('S');
            break;

        case '*':
            spin();
            uart_send_char('*');
            break;

        case 'T':
        	uart_send_char('T');
            self_test_mode();
            uart_send_char('B');
        	break;

        default:
            uart_send_char('?');
            break;
    }
}

