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
	int ID_max = 0;//最大的工号 
	char table[TABLE_LEN] = "employees";
	char buf[BUF_LEN] = "";
	char name[NAME_LEN] = "";
	char age[AGE_LEN] = "";
	char sex[SEX_LEN] = "";
	char addr[ADDR_LEN] = "";
	char phone[PHONE_LEN] = "";
	//char wage[WAGE_LEN] = "";
	char passwd[PASSWD_LEN] = "";

	strcpy(buf, pbuf);
	//获取最大行号，分配id,自动+1
	ID_max = get_table_maxID(db) + 1; 
	sscanf(buf+1,"%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]",name,age,sex,phone,addr,passwd);
	bzero(buf, sizeof(buf));
	sprintf(buf, "%c%c%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c", \
		'R', ID_max+48, '#', name, '#', age, '#', sex, '#', phone, '#', addr, '#', "?", '#', passwd, '#');
	//printf("buf[1]=%d %s:%s:__%d__\n",(int)buf[1],__FILE__,__func__,__LINE__);

	//将员工信息插入到数据库employees表中
	ret = do_insert(db, table, buf);
	if(ret < 0)
	{
		printf("注册错误\n");
		return -1;
	}
	//id为主键，不可重复注册，打印结果提示
	bzero(buf, sizeof(buf));
	if(0 == ret)
	{
		//注册成功  R+y+1
		sprintf(buf, "%c%c%c%c", 'R', 'y', 1, ID_max);
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

int login(int newfd, char *pbuf, sqlite3 *db, char *pid)
{
	int ret = 0;
	char sql[SQL_LEN] = "";
	char buf[BUF_LEN] = "";
	char table[TABLE_LEN] = "";
	char id[ID_LEN] = "";
	char passwd[PASSWD_LEN] = "";
	struct RESULT presult;
	char *errmsg = NULL;
	
	strcpy(buf, pbuf);
	strcpy(table, "employees");
	sscanf(buf+1, "%[^#]#%s", id, passwd);
	//将id和passwd与数据库中信息进行遍历比较
	ret = do_search(db, &presult, table, id);
	if(ret < 0)
	{
		//  L+n+4  "查无此人"
		sprintf(buf, "%c%c%c", 'L', 'n', 4);
		send_to_client(newfd, buf);
		return 0;
	}
	if(0 == strcmp(passwd, presult.result[16]))
	{
		if(0 == strcmp("off", presult.result[17]))
		{
			//两者都匹配无误，且status=off，则登录成功，打印提示，并将status=on，表示已登录
			//  L+y+1
			sprintf(buf, "%c%c%c", 'L', 'y', 1);
			strcpy(pid, id);//登录成功再把回调函数的id变量同步过来
			//  status=on
			sprintf(sql, "update employees set status=\"%s\" where id=\"%s\";", "on", id);
			ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
			if(ret != SQLITE_OK)
			{
				printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
				return -1;
			}
			printf("id=%s已登录,状态已修改\n", id);
		}else if(0 == strcmp("on", presult.result[17]))
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
	char flag = '0';
	int line = 0;
	int n = 9;//不要列名
	//time_t t_loc;
	char buf[BUF_LEN] = "";
	char table[TABLE_LEN] = "";
	char id[ID_LEN] = "";
	char name[NAME_LEN] = "";
	char age[AGE_LEN] = "";
	char sex[SEX_LEN] = "";
	char addr[ADDR_LEN] = "";
	char phone[PHONE_LEN] = "";
	char wage[WAGE_LEN] = "";
	char passwd[PASSWD_LEN] = "";
	char status[STATUS_LEN] = "";
	//char word[WORD_LEN] = "";
	//char mean[MEAN_LEN] = "";
	//char loc_time[TIME_LEN] = "";
	struct RESULT presult;
	//struct tm *info = NULL;

	strcpy(buf, pbuf);
	strcpy(table, "employees");
	sscanf(buf+1, "%[^#]#%c", id, &flag);//flag标志位，'$'管理员， '%'员工
	//fprintf(stdout, "buf=%s  __%d__\n", buf, __LINE__);

	//根据id，在数据库employees中进行查询记录
	ret = do_search(db, &presult, table, id);
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
			bzero(buf,sizeof(buf));
			bzero(id,sizeof(id));
			bzero(name,sizeof(name));
			bzero(age,sizeof(age));
			bzero(sex,sizeof(sex));
			bzero(phone,sizeof(phone));
			bzero(addr,sizeof(addr));
			bzero(wage,sizeof(wage));
			strcpy(id, presult.result[n++]);
			strcpy(name, presult.result[n++]);
			strcpy(age, presult.result[n++]);
			strcpy(sex, presult.result[n++]);
			strcpy(phone, presult.result[n++]);
			strcpy(addr, presult.result[n++]);
			strcpy(wage, presult.result[n++]);

			if(flag == '$')//管理员
			{
				bzero(passwd,sizeof(passwd));
				bzero(status,sizeof(status));
				strcpy(passwd, presult.result[n++]);
				strcpy(status, presult.result[n++]);
				//   S+y+id+#+name+#+age+#+sex+#+phone+#+addr+#+wage+#+passwd+#+status+#
				sprintf(buf, "%c%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c", \
					'S', 'y', id, '#', name, '#', age, '#', sex, '#', phone, '#', addr, '#', wage, '#', passwd, '#', status, '#');
			}else if(flag == '%')
			{
				n = n+2;//跳过passwd和status
				//   S+y+id+#+name+#+age+#+sex+#+phone+#+addr+#+wage+#
				sprintf(buf, "%c%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c", \
					'S', 'y', id, '#', name, '#', age, '#', sex, '#', phone, '#', addr, '#', wage, '#');
			}
			
			//打包发送给客户端
			send_to_client(newfd, buf);
			//fprintf(stdout, "*****buf=%s  __%s__   __%d__  *****\n", buf, __FILE__, __LINE__);

#if 0	//local_time			
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
#endif				
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

#if 0
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
#endif

int info_modify(int newfd, char *pbuf, sqlite3 *db)
{
	int ret = 0;
	char sql[SQL_LEN] = "";
	char buf[BUF_LEN] = "";
	char table[TABLE_LEN] = "";
	char id[ID_LEN] = "";
	char str[ADDR_LEN] = "";
	char key[10] = "";
	char choose = '0';
	struct RESULT presult;
	char *errmsg = NULL;

	strcpy(buf, pbuf);
	strcpy(table, "employees");
	sscanf(buf+1, "%[^#]#%[^#]#%[^#]", id, &choose, str);

	ret = do_search(db, &presult, table, id);
	if(ret < 0)
	{
		return -1;
	}
	switch(choose)
	{
		case '1':
			strcpy(key, "id");
			break;
		case '2':
			strcpy(key, "name");
			break;
		case '3':
			strcpy(key, "age");
			break;
		case '4':
			strcpy(key, "sex");
			break;
		case '5':
			strcpy(key, "phone");
			break;
		case '6':
			strcpy(key, "addr");
			break;
		case '7':
			strcpy(key, "wage");
			break;
		case '8':
			strcpy(key, "passwd");
			break;
		default:;
	}

	sprintf(sql, "update employees set %s=\"%s\" where id=\"%s\";", key, str, id);
	ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret != SQLITE_OK)
	{
		//   I+n+# 
		sprintf(buf, "%c%c%c", 'I', 'n', '#');
		printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}else
	{
		//   I+y+# 
		sprintf(buf, "%c%c%c", 'I', 'y', '#');
	}
	//打包发送给客户端
	send_to_client(newfd, buf);
	printf("信息已修改\n");

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
	char id[ID_LEN] = "";
	char old_passwd[PASSWD_LEN] = "";
	char new_passwd[PASSWD_LEN] = "";
	struct RESULT presult;
	char *errmsg = NULL;

	strcpy(buf, pbuf);
	strcpy(table, "employees");
	sscanf(buf+1, "%[^#]#%[^#]#%s", id, old_passwd, new_passwd);
	//根据id和old_passwd，比对数据库
	ret = do_search(db, &presult, table, id);
	if(ret < 0)
	{
		return -1;
	}
	if(0 == strcmp(old_passwd, presult.result[16]))
	{
		//   P+y+1  "ok"
		sprintf(buf, "%c%c%c", 'P', 'y', 1);
		//比对无误则更新passwd列内容为new_passwd
		sprintf(sql, "update employees set passwd=\"%s\" where id=\"%s\";", new_passwd, id);
		ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
		if(ret != SQLITE_OK)
		{
			printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
			return -1;
		}
		printf("id=%s密码已修改\n", id);
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
	sprintf(sql, "update employees set status=\"%s\" where id=\"%s\";", "off", buf+1);
	ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret != SQLITE_OK)
	{
		printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}
	printf("%s已退出登录,状态已修改\n", buf+1);
	return 0;
}

int recv_from_client(struct sockaddr_in cin, int newfd, char *buf, sqlite3 *db, char *id)
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
		sprintf(sql, "update employees set status=\"%s\" where id=\"%s\";", "off", id);
		ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
		if(ret != SQLITE_OK)
		{
			printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
			return -1;
		}
		printf("id=%s已退出登录,状态已修改\n", id);
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
