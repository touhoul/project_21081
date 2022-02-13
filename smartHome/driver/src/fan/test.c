#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>


#define FAN_MAGIC   'f'
#define CMD_LEVEL_1	 1
#define CMD_LEVEL_2	 2
#define CMD_LEVEL_3	 3
#define CMD_OFF      0
#define FAN_LEVEL_1	 _IOW(FAN_MAGIC, CMD_LEVEL_1, int)
#define FAN_LEVEL_2	 _IOW(FAN_MAGIC, CMD_LEVEL_2, int)
#define FAN_LEVEL_3  _IOW(FAN_MAGIC, CMD_LEVEL_3, int)
#define FAN_OFF 	 _IOW(FAN_MAGIC, CMD_OFF, int)

#define ERR_MSG(msg) ({printf("%s:%s:__%d__\n",__FILE__,__func__,__LINE__);perror(msg);})
#define PATH 		"/dev/myfan"

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
		if(ioctl(fd, FAN_LEVEL_1, &data) < 0)//FAN_LEVEL_1
		{
			ERR_MSG("ioctl error");
			return -1;
		}
		if(FAN_LEVEL_1 == data)
			printf("FAN_LEVEL_1 set okay\n");
		sleep(3);


		if(ioctl(fd, FAN_LEVEL_3, &data) < 0)//FAN_LEVEL_3
		{
			ERR_MSG("ioctl error");
			return -1;
		}
		if(FAN_LEVEL_3 == data)
			printf("FAN_LEVEL_3 set okay\n");
		sleep(3);

		if(ioctl(fd, FAN_LEVEL_2, &data) < 0)//FAN_LEVEL_2
		{
			ERR_MSG("ioctl error");
			return -1;
		}
		if(FAN_LEVEL_2 == data)
			printf("FAN_LEVEL_2 set okay\n");
		sleep(3);

		if(ioctl(fd, FAN_OFF, &data) < 0)//FAN_OFF
		{
			ERR_MSG("ioctl error");
			return -1;
		}
		if(FAN_OFF == data)
			printf("FAN_OFF set okay\n");
		sleep(3);
	}

	close(fd);
	
	return 0;
}
