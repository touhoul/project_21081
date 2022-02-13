#include "dev.h"


/*******     打开所需要的文件描述符Fd_tid           ********/
int openDevices(Fd_tid * my_fd_tid)
{
	Fd_tid fd_tid;
	//LED1
	fd_tid.fd_led1 = open("/dev/myAiLedCdev0", O_RDWR);
	if(fd_tid.fd_led1 < 0)
	{
		ERR_MSG("open");
		return -1;
	}


	//LED2
	fd_tid.fd_led2 = open("/dev/myAiLedCdev1", O_RDWR);
	if(fd_tid.fd_led2 < 0)
	{
		ERR_MSG("open");
		return -1;
	}

	//BEEP	
	fd_tid.fd_beep = open("/dev/mybeep", O_RDWR);
	if(fd_tid.fd_beep < 0)
	{
		ERR_MSG("open");
		return -1;
	}

	//si7006
	fd_tid.fd_si7006 = open("/dev/si7006_0", O_RDWR);
	if(fd_tid.fd_si7006 < 0)
	{
		ERR_MSG("open");
		return -1;
	}
	
	//m74hc595
	fd_tid.fd_m74hc595 = open("/dev/m74hc595", O_RDWR);
	if(fd_tid.fd_m74hc595 < 0)
	{
		ERR_MSG("open");
		//return -1;
	}

	//fan
	fd_tid.fd_fan = open("/dev/myfan", O_RDWR);
	if(fd_tid.fd_fan < 0)
	{
		ERR_MSG("open");
		return -1;
	}

	//MOTOR
	fd_tid.fd_motor = open("/dev/mymotor", O_RDWR);
	if(fd_tid.fd_motor < 0)
	{
		ERR_MSG("open");
		return -1;
	}

	fd_tid.flags = 0;
	*my_fd_tid = fd_tid;
	
	return 0;
	
}

int closeDevice(Fd_tid * fd_tid)
{
	close(fd_tid->fd_led1);
	close(fd_tid->fd_led2);
	close(fd_tid->fd_beep);
	close(fd_tid->fd_si7006);
	close(fd_tid->fd_fan);
	close(fd_tid->fd_motor);

	return 0;
	
}

//数据库初始化
int sqlite3Initialize(sqlite3 **mdb)
{
	int res;
	sqlite3 *db = *mdb;
	//创建打开数据库
	if(sqlite3_open("./AI.db", &db) != SQLITE_OK)
	{
		fprintf(stderr, "sqlite3_open: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	//创建表格
	char *errmsg = NULL;
	char sql[N] = "create table if not exists AI_HOME(Time char primary key, temp float, hump float);";
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("sqlite3_exec :%s line =%d\n", errmsg, __LINE__);
		return -1;
	}
	*mdb = db;
	//printf("表格创建完毕\n");

	return 0;	
}

void menu(void)
{
	printf("\t\t ***   l0 LED1_OFF ***\t\n");
	printf("\t\t ***   l1 LED1_ON  ***  \t\n");
	printf("\t\t ***   b0 BEEP_OFF  ***\t\n");
	printf("\t\t ***   b1 BEEP_ON  ***\t\n");
	printf("\t\t ***   f0 FAN_OFF  *** \t\n");
	printf("\t\t ***   f1 FAN_ON_1   ***\t\n");
	printf("\t\t ***   f2 FAN_ON_2   ***\t\n");
	printf("\t\t ***   f3 FAN_ON_3   ***\t\n");
	printf("\t\t ***   m0 MOTOR_OFF ***\t\n");
	printf("\t\t ***   m1 MOTOR_ON  ***  \t\n");
}

int judge(Fd_tid* fd_tid, char*buf)
{
	int ret;
	if(buf[1] == '0')
	{
		ioctl(fd_tid->fd_led1, LED1_OFF);
		ioctl(fd_tid->fd_beep, BEEP_OFF);
		ioctl(fd_tid->fd_fan, FAN_OFF);
		ioctl(fd_tid->fd_motor, MOTOR_OFF);
	}
	if(!strncasecmp(buf, "l0", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_led1,LED1_FLASH);
		ioctl(fd_tid->fd_led1,LED3_FLASH);
		ret = 0;
	}
	else if(!strncasecmp(buf, "l1", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_led1,CMD1_ON);
		ioctl(fd_tid->fd_led1,CMD_ON);
		LED1_ON;
		ret = 1;
	}
	else if(!strncasecmp(buf, "b0", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_beep, BEEP_OFF);
		ret = 0;
	}
	else if(!strncasecmp(buf, "b1", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_beep, BEEP_ON);
		ret = 1;
	}
	else if(!strncasecmp(buf, "f0", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_fan,FAN_OFF);
		ret = 0;
	}
	else if(!strncasecmp(buf, "f1", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_fan,FAN_LEVEL_1);
		ret = 1;
	}
	else if(!strncasecmp(buf, "f2", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_fan,FAN_LEVEL_2);
		ret = 1;
	}
	else if(!strncasecmp(buf, "f3", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_fan,FAN_LEVEL_3);
		ret = 1;
	}
	else if(!strncasecmp(buf, "m0", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_motor,MOTOR_OFF);
		ret = 0;
	}
	else if(!strncasecmp(buf, "m1", 2))
	{
		//		printf("***************%s******\n", buf);
		ioctl(fd_tid->fd_motor,MOTOR_ON);
		ret = 1;
	}
	return ret;
}
