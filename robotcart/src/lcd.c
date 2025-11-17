#define LCD_LIBRARY_VERSION
#ifdef LCD_LIBRARY_VERSION
/**
* Fixed LCD driver with correct pin mapping
* D0-D7 mapped to PA5-PA12
*/
#include "lcd.h"
#include "main.h"

#define MASK(x) (1UL << (x))

//pins
#define RS_PIN 0
#define EN 13 //PB13
#define D0 13
#define D1 6
#define D2 7
#define D3 8
#define D4 14
#define D5 15
#define D6 11
#define D7 12


//refine to not use busy delay
void delay_ms(int t_ms){
    for (int i=0; i<t_ms; i++)
        for (int j=0; j<1000;j++);
}

void lcd_gpio_init(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // RS, EN, D0–D7 as output push-pull
    uint8_t pins[] = {RS_PIN, D0, D1, D2, D3, D4, D5, D6, D7};
    for (int i = 0; i < sizeof(pins)/sizeof(pins[0]); i++) {
        uint8_t pin = pins[i];
        GPIOA->MODER &= ~(3 << (pin * 2));    // clear mode
        GPIOA->MODER |= (1 << (pin * 2));     // set as output
        GPIOA->OTYPER &= ~(1 << pin);         // push-pull
        GPIOA->OSPEEDR |= (3 << (pin * 2));   // high speed (optional)
    }

    //Configuring EN as PB13
    GPIOB->MODER &= ~(3 << (EN * 2));    // clear mode
    GPIOB->MODER |= (1 << (EN * 2));     // set as output
    GPIOB->OTYPER &= ~(1 << EN);         // push-pull
    GPIOB->OSPEEDR |= (3 << (EN * 2));   // high speed (optional)


}

// Helper function to write 8-bit data to correct pins
static void lcd_write_data_bus(uint8_t data)
{
    //
    uint32_t mask = (1<<D0) | (1<<D1) | (1<<D2) | (1<<D3) |
                    (1<<D4) | (1<<D5) | (1<<D6) | (1<<D7);
    GPIOA->ODR &= ~mask;

    // Map each bit of data to the correct pin
    if (data & 0x01) GPIOA->ODR |= (1 << D0);  // bit 0 -> PA5
    if (data & 0x02) GPIOA->ODR |= (1 << D1);  // bit 1 -> PA6
    if (data & 0x04) GPIOA->ODR |= (1 << D2);  // bit 2 -> PA7
    if (data & 0x08) GPIOA->ODR |= (1 << D3);  // bit 3 -> PA8
    if (data & 0x10) GPIOA->ODR |= (1 << D4);  // bit 4 -> PA9
    if (data & 0x20) GPIOA->ODR |= (1 << D5);  // bit 5 -> PA10
    if (data & 0x40) GPIOA->ODR |= (1 << D6);  // bit 6 -> PA11
    if (data & 0x80) GPIOA->ODR |= (1 << D7);  // bit 7 -> PA12
}

void lcd_enable_pulse(void)
{
    GPIOB->BSRR = (1U << EN);  // EN = 1
    delay_ms(1);                // pulse width
    GPIOB->BRR = (1U << EN);   // EN = 0
    delay_ms(1);
}

static void lcd_write_command(uint8_t command)
{
    delay_ms(2);
    GPIOA->BRR = (1U << RS_PIN);  // RS = 0 (instruction)
    GPIOB->BRR = (1U << EN);       // EN = 0

    // Write command to data bus
    lcd_write_data_bus(command);

    lcd_enable_pulse();
}

void lcd_write_char(unsigned char c)
{
    delay_ms(2);

    GPIOA->BSRR = (1U << RS_PIN);  // RS = 1 (data)
    GPIOB->BRR = (1U << EN);        // EN = 0

    // Write character to data bus
    lcd_write_data_bus(c);

    lcd_enable_pulse();
}

void lcd_init(void){
    lcd_gpio_init();
    delay_ms(40); // Wait >15ms after Vcc rises

    // Function set: 8-bit, 2 lines, 5x8 font
    lcd_write_command(0x38);
    delay_ms(5);

    // Display ON/OFF control: display on, cursor off, blink off
    lcd_write_command(0x0C);
    delay_ms(1);

    // Clear display
    lcd_write_command(0x01);
    delay_ms(2);

    // Entry mode set: increment cursor
    lcd_write_command(0x06);
}

void lcd_clear(void){
    delay_ms(2);
    lcd_write_command(CLEAR_MEMORY);   //Clear the DDRAM
    lcd_write_command(RESET_TO_HOME);  //returns home
}

void lcd_gotoXY(unsigned char column, unsigned char row)
{
    delay_ms(2);
    if(row==0)
        lcd_write_command(0x80 + column);
    if(row==1)
        lcd_write_command(0xC0 + column);
}

void lcd_print_string(char *s){
    delay_ms(2);
    while(*s != 0)
    {
        lcd_write_char(*s);
        s++;
    }
}

void lcd_clear_bottomRow(void){
	lcd_gotoXY(0, 1);
	lcd_print_string("                ");
}

#endif
