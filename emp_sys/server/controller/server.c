#include "./../model/model.h"
#include "./../sqlite3/my_sqlite.h"

struct CB
{
	int newfd;
	sqlite3 *db;
	struct sockaddr_in cin;
};

void* callback_mutual(void *arg);

int main(int argc, const char *argv[])
{
	int sfd = 0;
	int ret = 0;
	sqlite3 *db = NULL;
	int newfd = 0;
	char *c_ip;
	uint32_t c_port;
	pthread_t thread;
	socklen_t addrlen;
	struct sockaddr_in cin;
	struct CB info;

	//服务器初始化
	printf("-----服务器初始化中-----\n");
	sfd = init_server();
	if(sfd < 0)
	{
		printf("服务器初始化失败  __%d__\n", __LINE__);
		return -1;
	}
	printf("-----服务器初始化完成---\n");

	//网络初始化
	printf("-----网络初始化中-------\n");
	db = init_network();
	if(NULL == db)
	{
		printf("网络初始化失败  __%d__\n", __LINE__);
		return -1;
	}
	printf("-----网络初始化完成-----\n");

	//将套接字设置为被动监听状态
	if(listen(sfd, 10) < 0)
		ERR_MSG("listen", -1);
	printf("服务器监听中...\n");

	while(1)
	{
		//获取连接成功后的套接字
		addrlen = sizeof(cin);
		newfd = accept(sfd, (struct sockaddr*)&cin, &addrlen);
		if(newfd < 0)
			ERR_MSG("accept", -1);

		//客户端的地址信息
		c_port = ntohs(cin.sin_port);
		c_ip = inet_ntoa(cin.sin_addr);
		printf("[%s:%d]newfd=%d 链接成功\n",c_ip,c_port,newfd);

		info.newfd = newfd;
		info.db = db;
		info.cin = cin;
		ret = pthread_create(&thread, NULL, callback_mutual, &info);
		if(ret < 0)
			ERR_MSG("pthread_create", -1);
		pthread_detach(thread);
	}


	//关闭套接字
	close(sfd);

	return 0;
}

void* callback_mutual(void *arg)
{
	struct CB info = *(struct CB*)arg;
	int newfd = info.newfd;
	int ret = 0;
	sqlite3 *db = info.db;
	struct sockaddr_in cin = info.cin;;
	char buf[BUF_LEN] = "";
	char id[ID_LEN] = "";

	while(1)
	{
		bzero(buf, sizeof(buf));
		ret = recv_from_client(cin, newfd, buf, db, id);
		if(ret < 0)
		{
			return NULL;
		}
		switch(buf[0])
		{
		case 'R':
			do_register(newfd, buf, db);
			break;
		case 'L':
			login(newfd, buf, db, id);
			break;
		case 'D':
			ret = do_delete(db, "employees", buf);
			bzero(buf, sizeof(buf));
			if(ret != 0)
			{
				//   D+'n'+'#'  删除失败
				sprintf(buf, "%c%c%c", 'D', 'n', '#');
			}else
			{
				//   D+'y'+'#'  删除成功
				sprintf(buf, "%c%c%c", 'D', 'y', '#');
			}
			//打包发送给客户端
			send_to_client(newfd, buf);
			break;
		case 'S':
			search(newfd, buf, db);
			break;
		case 'I':
			info_modify(newfd, buf, db);
			break;			
		case 'P':
			passwd_modify(newfd, buf, db);
			break;
		case 'B':
			back(buf, db);
			bzero(id, sizeof(id));
			break;
		default:
			break;
		}
	}

	close(newfd);
	pthread_exit(NULL);
	return NULL;
}
