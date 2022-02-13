#include "dev.h"

void* pthread_get_data(void* arg);
void* pthread_use_data(void* arg);
void* pthread_show_data(void* arg);
void* pthread_inter_data(void* arg);
static int flags = 0;

//创建互斥锁
pthread_mutex_t fastmutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, const char* argv[])
{
	Fd_tid fd_tid;
	int ret;
	

	/************打开所有的文件描述符**************/
	ret = openDevices(&fd_tid);
	if(ret < 0)
	{
		//printf("openDevices error: file: %s  line: %d\n", __FILE__, __LINE__);
		return -1;
	}

	//printf("^^^^^^^^^^^^%d^^^^^ \n", flags);

	/************创建数据采集线程******************/
	pthread_create(&fd_tid.tid_get, NULL, pthread_get_data, &fd_tid);	
	ret = pthread_detach(fd_tid.tid_get);
	if(ret) 
	{
		perror("detach use data thread");
		exit(EXIT_FAILURE);
	} 
	else
	{
	//	printf("detach get data thread success\n");
	}

	/************创建数据处理线程******************/
	pthread_create(&fd_tid.tid_use, NULL, pthread_use_data, &fd_tid);
#ifndef test	
	ret = pthread_detach(fd_tid.tid_use);
	if(ret) 
	{
		perror("detach sue data thread");
		exit(EXIT_FAILURE);
	} 
	else
	{
	//	printf("detach use data thread success\n");
	}
#endif

	/************创建数据显示线程******************/
	pthread_create(&(fd_tid.tid_show), NULL, pthread_show_data, &fd_tid);	
#ifndef test
	ret = pthread_detach(fd_tid.tid_show);
	if(ret) 
	{
		perror("detach data show thread");
		exit(EXIT_FAILURE);
	} 
	else
	{
	//	printf("detach data show thread success\n");
	}
#endif

	/************创建用户交互线程******************/
	pthread_create(&fd_tid.tid_inter, NULL, pthread_inter_data, &fd_tid);	
#ifdef test
	//printf("11111\n");
	ret = pthread_detach(fd_tid.tid_inter);
	if(ret) 
	{
		perror("detach inter thread");
		exit(EXIT_FAILURE);
	} 
	else
	{
	//	printf("detach inter thread success\n");
	}
#else
	pthread_join(fd_tid.tid_inter, NULL);
#endif

	/************关闭所有的文件描述符**************/
	closeDevice(&fd_tid);

#if 0
	pthread_join(fd_tid.tid_get, NULL);
	pthread_join(fd_tid.tid_use, NULL);
	pthread_join(fd_tid.tid_show, NULL);
	pthread_join(fd_tid.tid_inter, NULL);
#endif
	return 0;
}

//读取温湿度数据
void* pthread_get_data(void* arg)
{
	Fd_tid fd_tid;
	int data;
	int ret;
	time_t t1;
	struct tm *info = NULL;
	char sql[N] = "";
	char str[N] = "";
	char *errmsg = NULL;
	sqlite3 *db;

	//创建数据库
	ret = sqlite3Initialize(&db);
	if(ret < 0)
	{
	//	printf("sqlite3Initialize failed\n");
		exit(0);
	}


	while(1)
	{
		pthread_mutex_lock(&fastmutex);
		fd_tid = *(Fd_tid *)arg;
		fd_tid.db = db;

	//	printf("1 pthread_get_data\n");
		ret = ioctl(fd_tid.fd_si7006, READ_TEMP, &data);
		if(ret < 0)
		{
	//		printf("ioctl failed, file: %s, line: %d\n", __FILE__,__LINE__);
			continue;
		}
		fd_tid.temp_rh.temp = 175.72 * data/65536.0 - 46.85;

		ret = ioctl(fd_tid.fd_si7006, READ_HUMP, &data);
		if(ret < 0)
		{
	//		printf("ioctl failed, file: %s, line: %d\n", __FILE__,__LINE__);
			continue;
		}
		fd_tid.temp_rh.hump = 125.0 * data / 65536.0 - 6.0;
		printf("temp: %.2f, hump= %.2f\n", fd_tid.temp_rh.temp, fd_tid.temp_rh.hump);
		
		//获取时间
		t1 = time(NULL);
		info = localtime(&t1);
		sprintf(str, "%4d-%02d-%02d %02d:%02d:%02d", info->tm_year+1900, info->tm_mon+1, info->tm_mday,\
				info->tm_hour, info->tm_min, info->tm_sec);
		sprintf(sql, "insert into AI_HOME values (\"%s\", \"%.2f\", \"%.2f\");",str, fd_tid.temp_rh.temp, fd_tid.temp_rh.hump);
		if(sqlite3_exec(fd_tid.db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
		{
	//		printf("sqlite3_exec: %s line: %d\n", errmsg, __LINE__);
			continue;
		}
		strcpy(fd_tid.temp_rh.time, str);

		*(Fd_tid *)arg = fd_tid;
		pthread_mutex_unlock(&fastmutex);
		sleep(1);
	}

}

void* pthread_use_data(void* arg)
{
	Fd_tid fd_tid;
	double temp;
	
	while(1)
	{
		pthread_mutex_lock(&fastmutex);
	//	printf("2 pthread_use_data\n");
		
		fd_tid = *(Fd_tid *)arg;
		temp = fd_tid.temp_rh.temp;
	//	printf("$$$$$$$$$$$$ %d $$$$$$$$ \n", flags);
		if(flags == 0)
		{
			if(temp >= MAX_TEMP)
				ALARM_START();
			else
				ALARM_STOP();
		}
		
		pthread_mutex_unlock(&fastmutex);
		sleep(1);
	}

}
void* pthread_show_data(void* arg)
{
	Fd_tid fd_tid;
	int ret, num;
	int temp, rh;

	while(1)
	{
		//pthread_mutex_lock(&fastmutex);
		//printf("3 pthread_show_data\n");

		fd_tid = *(Fd_tid *)arg;
		temp = (int)(fd_tid.temp_rh.temp *10);
		rh = (int)(fd_tid.temp_rh.hump *10);

		//printf("temp:%d  rh:%d\n",temp,rh);

		for(num=3000;num>=0;num--)
		{
			ret = ioctl(fd_tid.fd_m74hc595, SEG_TEM, temp);
			if(ret < 0)
			{
				printf("ioctl failed, file: %s, line: %d\n", __FILE__,__LINE__);
				continue;
			}
		}

		for(num=3000;num>=0;num--)
		{
			ret = ioctl(fd_tid.fd_m74hc595, SEG_RH, rh);
			if(ret < 0)
			{
				printf("ioctl failed, file: %s, line: %d\n", __FILE__,__LINE__);
				continue;
			}
		}

		//pthread_mutex_unlock(&fastmutex);
	}
}

void* pthread_inter_data(void* arg)
{
	Fd_tid fd_tid;
	menu();
	char buf[32];
	int ret;
	while(1)
	{
		fd_tid = *(Fd_tid*)arg;
		bzero(buf, sizeof(buf));
		scanf("%s", buf);

		ret = judge(&fd_tid, buf);

		flags = ret;
		if(flags == 1)
			printf("手动控制\n");
		else
			printf("自动控制\n");
	}
}


