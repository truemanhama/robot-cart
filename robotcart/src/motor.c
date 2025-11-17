#include "main.h"
#include "motor.h"
#include "lights.h"
#include "lcd.h"
#include <stdbool.h>


void gpio_init(void){
	//initializing the pins
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	GPIOB->MODER &= ~GPIO_MODER_MODER15;
	GPIOB->MODER |= GPIO_MODER_MODER15_0; //01 - general purpose output

	GPIOB->MODER &= ~GPIO_MODER_MODER2;
	GPIOB->MODER |= GPIO_MODER_MODER2_0;

	GPIOB->MODER &= ~GPIO_MODER_MODER3;
	GPIOB->MODER |= GPIO_MODER_MODER3_0;

	GPIOB->MODER &= ~GPIO_MODER_MODER4;
	GPIOB->MODER |= GPIO_MODER_MODER4_0;

}

void forward(void){
	lcd_clear_bottomRow();
	lcd_gotoXY(0, 1);
	lcd_print_string("Going Forward");
	lights(1);
	//function for the motor to go forward
	GPIOB->BSRR = (1<<15);//input 1 is on
	GPIOB->BSRR = (1<<18); //input 2 is off

	//right wheels
	GPIOB->BSRR = (1<<3); //input 3 is on
	GPIOB->BSRR = (1<<20); //input 4 is off
}

void backward(void){
	lcd_clear_bottomRow();
	lcd_gotoXY(0, 1);
	lcd_print_string("Going Backward");
	lights(2);
	//function for the motor to go forward
	GPIOB->BSRR = (1<<31);//input 1 is off
	GPIOB->BSRR = (1<<2); //input 2 is on

	//right wheels
	GPIOB->BSRR = (1<<19); //input 3 is off
	GPIOB->BSRR = (1<<4); //input 4 is on

}

void right(void){
	lcd_clear_bottomRow();
	lcd_gotoXY(0, 1);
	lcd_print_string("Turning Right");
	//left wheels stop - right wheels go forward
	//left wheels
	GPIOB->BSRR = (1<<15);
	GPIOB->BSRR = (1<<18);

	//right wheels
	GPIOB->BSRR = (1<<19);
	GPIOB->BSRR = (1<<20); //changed this to off make wheels spin in opposite direction

}

void left(void){
	lcd_clear_bottomRow();
	lcd_gotoXY(0, 1);
	lcd_print_string("Turning Left");
	//left wheels spin - right wheels stop
	//left wheels

	GPIOB->BSRR = (1<<31);
	GPIOB->BSRR = (1<<18); //changed this to off as well

	//right wheels
	GPIOB->BSRR = (1<<3);
	GPIOB->BSRR = (1<<20);

}

void stop(void){
	lcd_clear_bottomRow();
	lcd_gotoXY(0, 1);
	lcd_print_string("Stopping...");
	//all inputs 0
	//left wheels
	lights(2);
	GPIOB->BSRR = (1<<31);
	GPIOB->BSRR = (1<<18);

	//right wheels
	GPIOB->BSRR = (1<<19);
	GPIOB->BSRR = (1<<20);
}

void spin(void){
	lcd_clear_bottomRow();
	lcd_gotoXY(0, 1);
	lcd_print_string("Spinning...");
	lights(2);
	GPIOB->BSRR = (1<<31);
	GPIOB->BSRR = (1<<2); //changed this to make the motors spin opposite directions

	//right wheels
	GPIOB->BSRR = (1<<3);
	GPIOB->BSRR = (1<<20);
}

void self_test_mode(void) {
    // Clear previous states
    turning_left = false;
    turning_right = false;
    reversing = false;
    lcd_clear_bottomRow();
    lcd_gotoXY(0, 1);
    lcd_print_string("Self Test Mode");
    delay_ms(100);
    // Actions

	forward();
	delay_ms(3000);

	backward();
	reversing= true;
	delay_ms(3000);

	left();
	reversing= false;
	turning_right= true;
	delay_ms(3000);

	right();
	turning_left = true;
	turning_right = false;
	delay_ms(3000);

	spin();
	turning_left = false;
	delay_ms(3000);


	stop();
//        lights(5);  // hazards
	reversing= true;
	delay_ms(3000);
	reversing= false;



    stop();
    lights(0);
    lcd_clear_bottomRow();
    lcd_gotoXY(0, 1);
    lcd_print_string("Self Test Ended");
}


