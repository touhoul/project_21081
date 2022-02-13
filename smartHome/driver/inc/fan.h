#ifndef __FAN_H_
#define __FAN_H_

#include "stm32mp1xx_gpio.h"
#include "stm32mp1xx_rcc.h"
#include "stm32mp1xx_tim.h"



#define FAN_MAGIC   'f'
#define CMD_LEVEL_1	 1
#define CMD_LEVEL_2	 2
#define CMD_LEVEL_3	 3
#define CMD_OFF      0
#define FAN_LEVEL_1	 _IOW(FAN_MAGIC, CMD_LEVEL_1, int)
#define FAN_LEVEL_2	 _IOW(FAN_MAGIC, CMD_LEVEL_2, int)
#define FAN_LEVEL_3  _IOW(FAN_MAGIC, CMD_LEVEL_3, int)
#define FAN_OFF 	 _IOW(FAN_MAGIC, CMD_OFF, int)

#define GPIOE_PHY_BASE 	0x50006000 //(moder odr)

#define RCC_ENB      	0x50000000 //rcc_enb
#define TIM1_ENB  		0x44000000



//fan:	PE9--TIM1_CH1--pwm1--AF1

void fan_tim1_init(void);


#endif

































