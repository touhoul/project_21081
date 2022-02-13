#ifndef __DEV_H__
#define __DEV_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "sqlite3.h"

#define ERR_MSG(msg) do{\
	perror(msg);\
	printf("error line: %d error file: %s\n",__LINE__, __FILE__);\
}while(0)

#define M 128
#define MAX_TEMP 22
//led驱动接口
#define LED_MAGIC  'h'
#define CMD1_ON   1
#define CMD1_OFF   0
#define CMD1_FALSH 2
#define CMD_ON    3
#define CMD_OFF   4
#define CMD_FALSH 5

#define LED1_ON _IOW(LED_MAGIC, CMD1_ON, int)
#define LED1_OFF _IOW(LED_MAGIC, CMD1_OFF, int)
#define LED1_FLASH _IOW(LED_MAGIC, CMD1_FALSH, int)
#define LED3_ON _IOW(LED_MAGIC, CMD_ON, int)                                                                                                                                                                       
#define LED3_OFF _IOW(LED_MAGIC, CMD_OFF, int)
#define LED3_FLASH _IOW(LED_MAGIC, CMD_FALSH, int)


//蜂鸣器驱动接口
#define BEEP_MAGIC  'b'
#define BEEP_ON  _IOW(BEEP_MAGIC, CMD1_ON,  int)
#define BEEP_OFF _IOW(BEEP_MAGIC, CMD1_OFF, int)

//风扇驱动
#define FAN_MAGIC  'f'
#define CMD_LEVEL_1	1
#define CMD_LEVEL_2	2
#define CMD_LEVEL_3	3
#define FAN_LEVEL_1  _IOW(FAN_MAGIC, CMD_LEVEL_1, int)
#define FAN_LEVEL_2  _IOW(FAN_MAGIC, CMD_LEVEL_2, int)
#define FAN_LEVEL_3  _IOW(FAN_MAGIC, CMD_LEVEL_3, int)
#define FAN_OFF _IOW(FAN_MAGIC, CMD1_OFF, int)

//数码管
#define SEG_MAGIC  's'
#define CMD_SWICH	1
#define CMD_DATE	2
#define CMD_TEM	3
#define CMD_RH	4
#define SEG_WHICH _IOW(SEG_MAGIC, CMD_SWICH, int)
#define SEG_DAT  _IOW(SEG_MAGIC, CMD_DATE,  int)
#define SEG_TEM _IOW(SEG_MAGIC, CMD_TEM, int)
#define SEG_RH  _IOW(SEG_MAGIC, CMD_RH,  int)

//温湿度驱动接口
#define si7006_MAJIC 'w'
#define CMD_TEMP 1
#define CMD_HUMP 2
#define READ_TEMP _IOW(si7006_MAJIC, CMD_TEMP, int)
#define READ_HUMP _IOW(si7006_MAJIC, CMD_HUMP, int)

//马达驱动接口
#define MOTOR_MAJIC 'm'
#define MOTOR_ON  _IOW(MOTOR_MAJIC, CMD1_ON, int)
#define MOTOR_OFF _IOW(MOTOR_MAJIC, CMD1_OFF, int)

/*
 *	si7006接收到的数据通过该结构体进行传递
 */
#define N 512
typedef struct{
	char time[N];
	double temp;
	double hump;
}Temp_hump;

/*
 *	fd[0] led驱动
 *	fd[1] 蜂鸣器驱动
 *	fd[2] 温湿度驱动
 *	fd[3] 数码管驱动
 *	fd[4] 风扇驱动
 *	fd[5] 马达驱动
 *
 *	tid[0] 数据采集线程
 *	tid[1] 数据处理线程
 *	tid[2] 数据显示线程
 *	tid[3] 用户交互线程
 */

typedef struct{
	int fd_led1;
	int fd_led2;
	int fd_beep;
	int fd_si7006;
	int fd_m74hc595;
	int fd_fan;
	int fd_motor;

	pthread_t tid_get;
	pthread_t tid_use;
	pthread_t tid_show;
	pthread_t tid_inter;

	sqlite3 * db;

	int flags;

	Temp_hump temp_rh;

}Fd_tid;

/*****    温湿度超过阈值，风扇  ，蜂鸣器，LED灯，马达均会产生报警信号      			         	  ********/
#define ALARM_START() do{\
	ioctl(fd_tid.fd_led1, LED1_FLASH);\
	ioctl(fd_tid.fd_beep, BEEP_OFF);\
	ioctl(fd_tid.fd_fan, FAN_LEVEL_3);\
	ioctl(fd_tid.fd_motor, MOTOR_ON);\
}while(0)


/******   温湿度恢复到阈值，风扇  ，蜂鸣器，LED灯，马达报警信号消失  			                ********/
#define ALARM_STOP() do{\
	ioctl(fd_tid.fd_led1, LED1_OFF);\
	ioctl(fd_tid.fd_beep, BEEP_OFF);\
	ioctl(fd_tid.fd_fan, FAN_OFF);\
	ioctl(fd_tid.fd_motor, MOTOR_OFF);\
}while(0)


int sqlite3Initialize(sqlite3 **mdb);
int closeDevice(Fd_tid * fd_tid);
int openDevices(Fd_tid * my_fd_tid);
void menu(void);
int judge(Fd_tid* fd_tid, char*buf);

#endif
