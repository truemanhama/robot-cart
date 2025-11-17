/*
 * lcd.h
 *
 *  Created on: Nov 13, 2020
 *      Author: namanquah
 *      based on code by Steve Arar
 *      "How to Interface a 16×2 LCD Module with an MCU"
 *      https://www.allaboutcircuits.com/technical-articles/how-to-a-162-lcd-module-with-an-mcu/
 */

#ifndef LCD_H_
#define LCD_H_


/**commands**/
#define CURSOR_INCREMENT_MODE		(0x06)        	//Increment mode for the cursor
#define DISPLAYON_CURSOR_OFF 		(0x0C)        	//The display on, the cursor off
#define	EIGHT_BIT_DATA				(0x38)        	//An 8-bit data bus, two-line display
#define CLEAR_MEMORY				(0x01)			//clear screen
#define RESET_TO_HOME				(0x02)




//function summary
void lcd_setup_pins(void);
void lcd_init (void);
void lcd_clear(void);
void lcd_gotoXY(unsigned char column, unsigned char row);
void lcd_print_string(char *s);
void lcd_shift_right();
void lcd_shift_left();
void lcd_gpio_init(void);
void lcd_enable_pulse(void);
void lcd_write_char(unsigned char c);
void lcd_clear_bottomRow(void);

#endif /* LCD_H_ */
