/*
 * Headlights, Brake Lights and Indicators
 */
#include "main.h"
#include "lights.h"
#include <stdint.h>

volatile bool turning_left = false;
volatile bool turning_right = false;
volatile bool reversing = false;
static uint8_t turn_counter = 0;   // counts 100ms ticks
static uint8_t buzzer_counter = 0;

/**
 * Initialize GPIOC pins for lights as outputs
 */
void lights_init(void) {
    // Enable clock for GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Configure PC1–PC4 as outputs (assuming these pins are used)
    // Each pin uses 2 bits in MODER (00=input, 01=output)
    GPIOC->MODER &= ~(0x3 << (HEADLIGHTS * 2));
    GPIOC->MODER |=  (0x1 << (HEADLIGHTS * 2));

    GPIOC->MODER &= ~(0x3 << (BRAKELIGHTS * 2));
    GPIOC->MODER |=  (0x1 << (BRAKELIGHTS * 2));

    GPIOC->MODER &= ~(0x3 << (LEFT * 2));
    GPIOC->MODER |=  (0x1 << (LEFT * 2));

    GPIOC->MODER &= ~(0x3 << (RIGHT * 2));
    GPIOC->MODER |=  (0x1 << (RIGHT * 2));

    GPIOC->MODER &= ~(0x3 << (BUZZER * 2));
    GPIOC->MODER |=  (0x1 << (BUZZER * 2));

    // Set output type to push-pull
    GPIOC->OTYPER &= ~((1 << HEADLIGHTS) | (1 << BRAKELIGHTS) | (1 << LEFT) | (1 << RIGHT) | (1<<BUZZER));

    // No pull-up/pull-down
    GPIOC->PUPDR &= ~((0x3 << (HEADLIGHTS * 2)) | (0x3 << (BRAKELIGHTS * 2)) | (0x3 << (LEFT * 2)) | (0x3 << (RIGHT * 2)) | (3<<(BUZZER*2)));

    // Turn all lights off initially
    GPIOC->ODR &= ~((1 << HEADLIGHTS) | (1 << BRAKELIGHTS) | (1 << LEFT) | (1 << RIGHT) | (1<<BUZZER));
}

void onboard_led_init(void) {
    // Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Configure PA5 as output (onboard LED)
    GPIOA->MODER &= ~(0x3 << (5 * 2));  // clear mode bits
    GPIOA->MODER |=  (0x1 << (5 * 2));  // set as general output
    GPIOA->OTYPER &= ~(1 << 5);         // push-pull
    GPIOA->PUPDR  &= ~(0x3 << (5 * 2)); // no pull-up/down

    // Led off initially
    GPIOA->BRR = (1 << 5);
}


/**
 Controlling which lights are on.
 */
void lights(uint8_t x) {
    // Ensure pins are initialized
    lights_init();

    // First, turn all off
    GPIOC->ODR &= ~((1 << BRAKELIGHTS) | (1 << LEFT) | (1 << RIGHT));

    switch (x) {
    	case 1:
    		GPIOC->ODR |= (1<<HEADLIGHTS);
    		break;

        case 2: // Brake lights
            GPIOC->ODR |= (1 << BRAKELIGHTS);
            break;

        case 3: // Left indicator
            GPIOC->ODR |= (1 << LEFT);
            break;

        case 4: // Right indicator
            GPIOC->ODR |= (1 << RIGHT);
            break;

        case 5: // Both indicators (hazard lights)
        	GPIOC->ODR |= (1 << RIGHT);
        	GPIOC->ODR |= (1 << LEFT);
            break;

        default: // All off
            GPIOC->ODR &= ~((1 << BRAKELIGHTS) | (1 << LEFT) | (1 << RIGHT));
            break;
    }
}

void buzzer(int x){
	if(x == 1)
		GPIOC->ODR |= (1 << BUZZER);
	else
		GPIOC->ODR &= ~(1 << BUZZER);

}

void timer6_init(void) {
    // Enable Timer 6 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    TIM6->PSC = 7999;           // 1 ms per tick
    TIM6->ARR = 100 - 1;        // 100 ms period
    TIM6->EGR |= TIM_EGR_UG;    // Generate update event to load registers
    TIM6->DIER |= TIM_DIER_UIE; // Enable update interrupt
    TIM6->CR1  |= TIM_CR1_CEN;  // Start timer

    NVIC_EnableIRQ(TIM6_DAC_IRQn);  // Enable TIM6 interrupt in NVIC
}

void TIM6_DAC_IRQHandler(void) {
    static uint8_t led_counter = 0;   // counts 100 ms ticks → toggles PA5 every 200 ms

    if (TIM6->SR & TIM_SR_UIF) {      // Check update interrupt flag
        TIM6->SR &= ~TIM_SR_UIF;      // Clear flag

        // 200 ms Onboard LED toggle (PA5)
        led_counter++;
        if (led_counter >= 2) {       // 2 × 100 ms = 200 ms
            led_counter = 0;
            GPIOA->ODR ^= (1 << 5);   // Toggle PA5
        }

        //Handle turning lights (flash every 500 ms)
        if (turning_left || turning_right) {
            turn_counter++;
            if (turn_counter >= 5) {  // 5 × 100 ms = 500 ms
                turn_counter = 0;
                if (turning_left)
                    GPIOC->ODR ^= (1 << LEFT);   // Toggle left indicator
                if (turning_right)
                    GPIOC->ODR ^= (1 << RIGHT);  // Toggle right indicator
            }
        } else {
            GPIOC->ODR &= ~((1 << LEFT) | (1 << RIGHT)); // Ensure off
            turn_counter = 0;
        }

        //Handle reversing buzzer (beep every 1s)
        if (reversing) {
            buzzer_counter++;
            if (buzzer_counter >= 10) {  // 10 × 100 ms = 1 s
                buzzer_counter = 0;
                GPIOC->ODR ^= (1 << BUZZER);    // Toggle buzzer
            }
        } else {
            GPIOC->ODR &= ~(1 << BUZZER);
            buzzer_counter = 0;
        }
    }
}


//For the heartbeat functionality
static const bool heartbeat_pattern[14] = {
    1,0,1,0,1,0,0,0,1,0,1,0,0,0
};

static uint8_t pattern_index = 0;

void cartled_init(void) {
    // Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Configure CARTLED as output
    GPIOC->MODER &= ~(0x3 << (CARTLED * 2));
    GPIOC->MODER |=  (0x1 << (CARTLED * 2));
    GPIOC->OTYPER &= ~(CARTLED << 1);
    GPIOC->PUPDR  &= ~(0x3 << (CARTLED * 2));

    // Start with LED off
    GPIOC->BRR = (1 << 1);
}

void timer15_init(void) {
    // Enable GPIOA and Timer 15 clocks
   // RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;

    //Configure PA1 as AF5 (TIM15_CH1)
    GPIOA->MODER &= ~(3 << (1 * 2));
    GPIOA->MODER |=  (2 << (1 * 2));       // AF mode
    GPIOA->AFR[0] &= ~(0xF << (1 * 4));
    GPIOA->AFR[0] |=  (5 << (1 * 4));      // AF5 for TIM15

    //Timer base configuration
    TIM15->PSC = 7999;        // 8 MHz / (7999 + 1) = 1 kHz (1 ms tick)
    TIM15->ARR = 100 - 1;     // 100 ms update
    TIM15->CR1 &= ~TIM_CR1_DIR;  // Upcounting
    TIM15->EGR |= TIM_EGR_UG;    // Force update

    //Enable update interrupt
    TIM15->SR &= ~TIM_SR_UIF;     // Clear any pending flag
    TIM15->DIER |= TIM_DIER_UIE;  // Enable update interrupt

    //Enable interrupt in NVIC
    NVIC_EnableIRQ(TIM15_IRQn);

    //Start timer
    TIM15->CR1  |= TIM_CR1_CEN;
}


void TIM15_IRQHandler(void) {
    if (TIM15->SR & TIM_SR_UIF) {
        TIM15->SR &= ~TIM_SR_UIF;

        // Output next pattern step
        if (heartbeat_pattern[pattern_index])
            GPIOC->BSRR = (1 << CARTLED);   // ON
        else
            GPIOC->BRR  = (1 << CARTLED);   // OFF

        // Advance index
        pattern_index++;
        if (pattern_index >= sizeof(heartbeat_pattern))
            pattern_index = 0;
    }
}



