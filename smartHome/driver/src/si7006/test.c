#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define si7006_MAJIC 'w'
#define CMD_TEMP 1
#define CMD_HUMP 2
#define READ_TEMP _IOW(si7006_MAJIC, CMD_TEMP, int)
#define READ_HUMP _IOW(si7006_MAJIC, CMD_HUMP, int)


int main(int argc, const char *argv[])
{
	int temp, hum;
	float r_t, r_h;
	int fd = open("/dev/si7006_0", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}

	while(1)
	{
		ioctl(fd, READ_TEMP, &temp);
		ioctl(fd, READ_HUMP, &hum);
		r_h = (125.0*hum)/65536.0 - 6;
		r_t = (175.72*temp)/65536 - 46.85;
		printf("temp  %.2f  hump  %.2f  \n", r_t, r_h);
		sleep(1);
	}

	close(fd);
	return 0;
}
