#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MOTOR_MAGIC  'm'
#define CMD_ON	1
#define CMD_OFF 0
#define MOTOR_ON _IOW(MOTOR_MAGIC, CMD_ON, int)
#define MOTOR_OFF _IOW(MOTOR_MAGIC, CMD_OFF, int)

#define ERR_MSG(msg) ({printf("%s:%s:__%d__\n",__FILE__,__func__,__LINE__);perror(msg);})
#define PATH 		"/dev/mymotor"

int main(int argc, const char *argv[])
{
	int fd;
	int data;

	fd = open(PATH, O_RDWR);
	if(fd < 0)
	{
		ERR_MSG("open error");
		return -1;
	}
	
	while(1)
	{
		if(ioctl(fd, MOTOR_ON, &data) < 0)//MOTOR_ON
		{
			ERR_MSG("ioctl error");
			return -1;
		}
		if(MOTOR_ON == data)
			printf("MOTOR_ON set okay\n");
		sleep(3);

		if(ioctl(fd, MOTOR_OFF, &data) < 0)//MOTOR_OFF
		{
			ERR_MSG("ioctl error");
			return -1;
		}
		if(MOTOR_OFF == data)
			printf("MOTOR_OFF set okay\n");
		sleep(3);
	}

	close(fd);
	
	return 0;
}
