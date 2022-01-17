#include "./model.h"



int init_server()
{
	int sfd = 0;
	int reuse = 1;
	int ret = 0;
	socklen_t addrlen;
	struct sockaddr_in sin;

	//创建流式套接字
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0)
		ERR_MSG("socket", -1);
	//允许端口快速重用
	ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	if(ret < 0)
		ERR_MSG("setsockopt", -1);
	//填充地址信息结构体
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = inet_addr(IP);
	addrlen = sizeof(sin);
	//绑定服务器的端口和IP
	ret = bind(sfd, (struct sockaddr*)&sin, addrlen);
	if(ret < 0)
		ERR_MSG("bind", -1);
	printf("绑定成功...\n");
	
	return sfd;
}

int do_register(int newfd, char *pbuf, sqlite3 *db)
{
	int ret = 0;
	char table[TABLE_LEN] = "usr";
	char buf[BUF_LEN] = "";

	strcpy(buf, pbuf);
	//将name和passwd插入到数据库usr表中
	ret = do_insert(db, table, buf);
	if(ret < 0)
	{
		printf("注册错误\n");
		return -1;
	}
	//name为主键，不可重复注册，打印结果提示
	bzero(buf, sizeof(buf));
	if(0 == ret)
	{
		//注册成功  R+y+1
		sprintf(buf, "%c%c%c", 'R', 'y', 1);
		//printf("ack=%s  __%d__\n", buf, __LINE__);
	}else if(1 == ret)
	{
		//用户名已存在  R+n+2
		sprintf(buf, "%c%c%c", 'R', 'n', 2);
		//printf("ack=%s  __%d__\n", buf, __LINE__);
	}
	//发包给客户端
	send_to_client(newfd, buf);
	//printf("ack=%s  __%d__\n", buf, __LINE__);

	return 0;
}


int login(int newfd, char *pbuf, sqlite3 *db, char *pname)
{
	int ret = 0;
	char sql[SQL_LEN] = "";
	char buf[BUF_LEN] = "";
	char table[TABLE_LEN] = "";
	char name[NAME_LEN] = "";
	char passwd[PASSWD_LEN] = "";
	struct RESULT presult;
	char *errmsg = NULL;
	
	strcpy(buf, pbuf);
	strcpy(table, "usr");
	sscanf(buf+1, "%[^#]#%s", name, passwd);
	//将name和passwd与数据库中信息进行遍历比较
	ret = do_search(db, &presult, table, name);
	if(ret < 0)
	{
		//  L+n+4  "查无此人"
		sprintf(buf, "%c%c%c", 'L', 'n', 4);
		send_to_client(newfd, buf);
		return 0;
	}
	if(0 == strcmp(passwd, presult.result[4]))
	{
		if(0 == strcmp("off", presult.result[5]))
		{
			//两者都匹配无误，且status=off，则登录成功，打印提示，并将status=on，表示已登录
			//  L+y+1
			sprintf(buf, "%c%c%c", 'L', 'y', 1);
			strcpy(pname, name);//登录成功再把回调函数的name变量同步过来
			//  status=on
			sprintf(sql, "update usr set status=\"%s\" where name=\"%s\";", "on", name);
			ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
			if(ret != SQLITE_OK)
			{
				printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
				return -1;
			}
			printf("%s已登录,状态已修改\n", name);
		}else if(0 == strcmp("on", presult.result[5]))
		{
			//  L+n+3  "重复登录"
			sprintf(buf, "%c%c%c", 'L', 'n', 3);
		}
	}else
	{
			//  L+n+5  "密码错误"
			sprintf(buf, "%c%c%c", 'L', 'n', 5);
	}
	//发包给客户端
	send_to_client(newfd, buf);
	if(NULL != presult.result)
		sqlite3_free_table(presult.result);

	return 0;
}

int search(int newfd, char *pbuf, sqlite3 *db)
{
	int ret = 0;
	int line = 0;
	int n = 2;//不要列名
	time_t t_loc;
	char buf[BUF_LEN] = "";
	char table[TABLE_LEN] = "";
	char name[NAME_LEN] = "";
	char word[WORD_LEN] = "";
	char mean[MEAN_LEN] = "";
	char loc_time[TIME_LEN] = "";
	struct RESULT presult;
	struct tm *info = NULL;


	strcpy(buf, pbuf);
	strcpy(table, "dict");
	sscanf(buf+1, "%[^#]#%[^#]", name, word);
	//fprintf(stdout, "buf=%s  __%d__\n", buf, __LINE__);

	//根据word，在数据库dict中进行查询记录
	ret = do_search(db, &presult, table, word);
	if(ret < 0)
	{
		//   S+n+6  "未查找到结果"
		sprintf(buf, "%c%c%c", 'S', 'n', 6);
		//打包发送给客户端
		send_to_client(newfd, buf);
		return -1;
	}else
	{
		for(line=0; line<presult.line; line++)
		{
				bzero(word,sizeof(word));
				bzero(mean,sizeof(mean));
				bzero(buf,sizeof(buf));
				strcpy(word, presult.result[n++]);
				strcpy(mean, presult.result[n++]);
				//   S+y+word+#+mean+#
				sprintf(buf, "%c%c%s%c%s%c", 'S', 'y', word, '#', mean, '#');
				//打包发送给客户端
				send_to_client(newfd, buf);
				//fprintf(stdout, "*****buf=%s  __%s__   __%d__  *****\n", buf, __FILE__, __LINE__);
				//获取当前时间
				time(&t_loc);
				info = localtime(&t_loc);
				if(NULL == info)
					ERR_MSG("localtime", -1);

				bzero(loc_time,sizeof(loc_time));
				sprintf(loc_time, "[%d-%02d-%02d %02d:%02d:%02d]", \
						info->tm_year+1900, info->tm_mon+1, info->tm_mday,\
						info->tm_hour, info->tm_min, info->tm_sec);
				//  time+#+name+#+word+#+mean+#
				bzero(buf,sizeof(buf));
				sprintf(buf, "%s%c%s%c%s%c%s%c", loc_time, '#', name, '#', word, '#', mean, '#');
				//将time、name、word、mean存入history表格
				strcpy(table, "history");
				ret = do_insert(db, table, buf);
				if(ret < 0)
				{
					printf("历史记录插入错误\n");
					return -1;
				}
		}
		//   S+'#'  全部结果发送完毕
		sprintf(buf, "%c%c", 'S', '#');
		//打包发送给客户端
		send_to_client(newfd, buf);
	}
	if(NULL != presult.result)
		sqlite3_free_table(presult.result);
	return 0;
}

int history(int newfd, char *pbuf, sqlite3 *db)
{
	int ret = 0;
	int line = 0;
	int n = 4;//不要列名
	char buf[BUF_LEN] = "";
	char table[TABLE_LEN] = "";
	char name[NAME_LEN] = "";
	char word[WORD_LEN] = "";
	char mean[MEAN_LEN] = "";
	char loc_time[TIME_LEN] = "";
	struct RESULT presult;

	strcpy(buf, pbuf);
	strcpy(name, buf+1);
	strcpy(table, "history");
	//根据name，在数据库dict中进行查询记录
	ret = do_search(db, &presult, table, name);
	if(ret < 0)
	{
		//   H+n+7  "无历史记录"
		sprintf(buf, "%c%c%c", 'H', 'n', 7);
		//打包发送给客户端
		send_to_client(newfd, buf);
		return -1;
	}
	else
	{
		for(line=0; line<presult.line; line++)
		{
			bzero(loc_time,sizeof(loc_time));
			bzero(name,sizeof(name));
			bzero(word,sizeof(word));
			bzero(mean,sizeof(mean));
			bzero(buf,sizeof(buf));
			strcpy(loc_time, presult.result[n++]);
			strcpy(name, presult.result[n++]);
			strcpy(word, presult.result[n++]);
			strcpy(mean, presult.result[n++]);
			//   H+y+time+#+name+#+word+#+mean+#
			sprintf(buf, "%c%c%s%c%s%c%s%c%s%c", 'H', 'y', loc_time, '#', name, '#', word, '#', mean, '#');
			//打包发送给客户端
			send_to_client(newfd, buf);
		}
	}
	//   H+'#'  全部结果发送完毕
	sprintf(buf, "%c%c", 'H', '#');
	//打包发送给客户端
	send_to_client(newfd, buf);
	if(NULL != presult.result)
		sqlite3_free_table(presult.result);
	return 0;
}

int passwd_modify(int newfd, char *pbuf, sqlite3 *db)
{
	int ret = 0;
	char sql[SQL_LEN] = "";
	char buf[BUF_LEN] = "";
	char table[TABLE_LEN] = "";
	char name[NAME_LEN] = "";
	char old_passwd[PASSWD_LEN] = "";
	char new_passwd[PASSWD_LEN] = "";
	struct RESULT presult;
	char *errmsg = NULL;

	strcpy(buf, pbuf);
	strcpy(table, "usr");
	sscanf(buf+1, "%[^#]#%[^#]#%s", name, old_passwd, new_passwd);
	//根据name和old_passwd，比对数据库
	ret = do_search(db, &presult, table, name);
	if(ret < 0)
	{
		return -1;
	}
	if(0 == strcmp(old_passwd, presult.result[4]))
	{
		//   P+y+1  "ok"
		sprintf(buf, "%c%c%c", 'P', 'y', 1);
		//比对无误则更新passwd列内容为new_passwd
		sprintf(sql, "update usr set passwd=\"%s\" where name=\"%s\";", new_passwd, name);
		ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
		if(ret != SQLITE_OK)
		{
			printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
			return -1;
		}
		printf("%s密码已修改\n", name);
	}
	else
	{
		//   P+n+5  "密码输入有误"
		sprintf(buf, "%c%c%c", 'P', 'n', 5);
		//打包发送给客户端
		send_to_client(newfd, buf);
		return -1;
	}
	//打包发送给客户端
	send_to_client(newfd, buf);
	if(NULL != presult.result)
		sqlite3_free_table(presult.result);
	return 0;
}

int back(char *pbuf, sqlite3 *db)
{
	int ret = 0;
	char sql[SQL_LEN] = "";
	char buf[BUF_LEN] = "";
	char *errmsg = NULL;

	strcpy(buf, pbuf);
	//  status=off
	sprintf(sql, "update usr set status=\"%s\" where name=\"%s\";", "off", buf+1);
	ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret != SQLITE_OK)
	{
		printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}
	printf("%s已退出登录,状态已修改\n", buf+1);
	return 0;
}

int recv_from_client(struct sockaddr_in cin, int newfd, char *buf, sqlite3 *db, char *name)
{
	int ret = 0;
	ssize_t res;
	char *c_ip;
	uint32_t c_port;
	char sql[SQL_LEN] = "";
	//char name[NAME_LEN] = "";
	char *errmsg = NULL;

	//接受客户端信息
	res = recv(newfd, buf, BUF_LEN, 0);
	//fprintf(stdout, "buf=%s  __%d__\n", buf, __LINE__);
	//客户端的地址信息
	c_port = ntohs(cin.sin_port);
	c_ip = inet_ntoa(cin.sin_addr);
	if(res < 0)
		ERR_MSG("recv", -1);
	if(0 == res)
	{
		printf("[%s:%d]newfd=%d 客户端已退出\n",c_ip,c_port,newfd);
		//  status=off
		sprintf(sql, "update usr set status=\"%s\" where name=\"%s\";", "off", name);
		ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
		if(ret != SQLITE_OK)
		{
			printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
			return -1;
		}
		printf("%s已退出登录,状态已修改\n", name);
		return -1;
	}
	//printf("cfd=%d:%s  __%d__\n", newfd, buf, __LINE__);
	return 0;
}

int send_to_client(int newfd, char *buf)
{
	ssize_t res;
	//发送信息到客户端
	res = send(newfd, buf, BUF_LEN, 0);
	if(res < 0)
		ERR_MSG("send", -1);
	return 0;
}
