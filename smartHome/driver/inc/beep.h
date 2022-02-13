#ifndef __BEEP_H_
#define __BEEP_H_

#include "stm32mp1xx_gpio.h"
#include "stm32mp1xx_rcc.h"
#include "stm32mp1xx_tim.h"

//beep:	PB6--TIM4_CH1--pwm--AF2


#define BEEP_MAGIC  'b'
#define CMD_ON	1
#define CMD_OFF 0
#define BEEP_ON _IOW(BEEP_MAGIC, CMD_ON, int)
#define BEEP_OFF _IOW(BEEP_MAGIC, CMD_OFF, int)


#define GPIOB_PHY_BASE 	0x50003000 //(moder odr)

#define RCC_ENB      	0x50000000 //rcc_enb
#define TIM4_ENB  		0x40002000


void beep_tim4_init(void);


#endif

































