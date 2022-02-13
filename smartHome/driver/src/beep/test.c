#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define BEEP_MAGIC  'b'
#define CMD_ON	1
#define CMD_OFF 0
#define BEEP_ON _IOW(BEEP_MAGIC, CMD_ON, int)
#define BEEP_OFF _IOW(BEEP_MAGIC, CMD_OFF, int)

#define ERR_MSG(msg) ({printf("%s:%s:__%d__\n",__FILE__,__func__,__LINE__);perror(msg);})
#define PATH 		"/dev/mybeep"

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
		if(ioctl(fd, BEEP_ON, &data) < 0)//BEEP_ON
		{
			ERR_MSG("ioctl error");
			return -1;
		}
		if(BEEP_ON == data)
			printf("BEEP_ON set okay\n");
		sleep(3);

		if(ioctl(fd, BEEP_OFF, &data) < 0)//BEEP_OFF
		{
			ERR_MSG("ioctl error");
			return -1;
		}
		if(BEEP_OFF == data)
			printf("BEEP_OFF set okay\n");
		sleep(3);
	}

	close(fd);
	
	return 0;
}
