#include "main.h"
#include "lcd.h"
#include "lights.h"
#include "comms.h"
#include "motor.h"

int main(void)
{
    lcd_init();
    delay_ms(100);
    lcd_clear();
    lcd_gotoXY(0,0);
    lcd_print_string("BAT Mobile");
    lcd_gotoXY(0,1);
    lcd_print_string("Initializing...");

    // Initializing
    uart_init();
    gpio_init();
    lights_init();     // configuring for lights and buzzer
    onboard_led_init();
    timer6_init();     // start TIM6 interrupt for blinking/buzzer timing
    cartled_init();
    timer15_init();

    while (1) {
        // Wait for command via UART
        char cmd = uart_receive_char();
        // Process command
        uart_handle_command(cmd);
        // Small delay for input stability
        delay_ms(5);
    }
}
