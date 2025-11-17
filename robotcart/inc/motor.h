#include "main.h"
#include <stdbool.h>

#ifndef MOTOR_H_
#define MOTOR_H_

extern volatile bool self_t est_active;


void gpio_init(void);
void forward(void);
void backward(void);
void left(void);
void right(void);
void spin(void);
void stop(void);
void delayy(int t);
void self_test_mode(void);



#endif
