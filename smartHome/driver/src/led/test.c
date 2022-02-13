#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

//led驱动接口
#define LED_MAGIC  'h'
#define CMD1_ON	  1
#define CMD1_OFF   0
#define CMD1_FALSH 2
#define CMD_ON	  3
#define CMD_OFF   4
#define CMD_FALSH 5

#define LED1_ON _IOW(LED_MAGIC, CMD1_ON, int)
#define LED1_OFF _IOW(LED_MAGIC, CMD1_OFF, int)
#define LED1_FLASH _IOW(LED_MAGIC, CMD1_FALSH, int)
#define LED3_ON _IOW(LED_MAGIC, CMD_ON, int)
#define LED3_OFF _IOW(LED_MAGIC, CMD_OFF, int)
#define LED3_FLASH _IOW(LED_MAGIC, CMD_FALSH, int)
int main(int argc, const char *argv[])
{
	int fd1;
	int fd2;
	int data=0;
	printf("===================\n");
	if((fd1 = open("/dev/myAiLedCdev0", O_RDWR)) < 0)
	{
		perror("open");
		return -1;
	}
	if((fd2 = open("/dev/myAiLedCdev1", O_RDWR)) < 0)
	{
		perror("open");
		return -1;
	}
	while(1)
	{
		ioctl(fd1, LED1_ON, &data);
		ioctl(fd2, LED3_ON, &data);
		sleep(5);
		ioctl(fd1, LED1_OFF, &data);
		ioctl(fd2, LED3_OFF, &data);
		sleep(5);
		ioctl(fd1, LED1_FLASH, &data);
		ioctl(fd2, LED3_FLASH, &data);
		sleep(5);

	}
	return 0;
}
