#ifndef LIGHTS_H_
#define LIGHTS_H_

#include <stdbool.h>

extern volatile bool turning_left;
extern volatile bool turning_right;
extern volatile bool reversing;


#define HEADLIGHTS   (9)
#define BRAKELIGHTS  (8)
#define LEFT         (6)
#define RIGHT        (5)
#define STATUSLIGHTS (5)
#define BUZZER		(13)
#define CARTLED		(1)

void lights_init(void); //initializes the pins
void lights(uint8_t x);
void delay_ms(uint16_t ms);
void timer6_init(void);
void buzzer(int x);
void onboard_led_init(void);
void timer14_init(void);
void cartled_init(void);
void timer15_init(void);

#endif
