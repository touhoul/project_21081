#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "stm32mp1xx_gpio.h"
#include "stm32mp1xx_rcc.h"
#include "stm32mp1xx_tim.h"

//motor:	PF6--TIM16_CH1--pwm--AF1


#define MOTOR_MAGIC  'm'
#define CMD_ON	1
#define CMD_OFF 0
#define MOTOR_ON _IOW(MOTOR_MAGIC, CMD_ON, int)
#define MOTOR_OFF _IOW(MOTOR_MAGIC, CMD_OFF, int)


#define GPIOF_PHY_BASE 	0x50007000 //(moder odr)

#define RCC_ENB      	0x50000000 //rcc_enb
#define TIM16_ENB  		0x44007000


void motor_tim16_init(void);


#endif

































