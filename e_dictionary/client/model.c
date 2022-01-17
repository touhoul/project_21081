#include "./model.h"


int loop_main(int sfd)
{
	int choose = 0;
	int exit_flag = 0;
	int status = -1;
	char name[NAME_LEN] = "";

	while(1)
	{
		printf("***********************\n");
		printf("********1.注册*********\n");
		printf("********2.登录*********\n");
		printf("********3.退出*********\n");
		printf("***********************\n");
		printf("请输入指令>>>");
		fflush(stdout);
		choose = getchar();
		while(getchar() != 10);
		switch(choose)
		{
		case '1':
			do_register(sfd);
			break;
		case '2':
			status = login(sfd, name);
			break;
		case '3':
			exit_flag = 1;
			break;
		default:
			printf("输入有误\n");
			printf("输入任意字符清屏: ");
			fflush(stdout);
			while(getchar()!=10);
			system("clear");
			continue;
		}
		if(exit_flag)
			break;
		if(0 == status)
			loop_usr(sfd, &status, name);
	}

	return 0;
}

int loop_usr(int sfd, int *status, char *name)
{
	int choose = 0;
	int back_flag = 0;

	while(1)
	{
		printf("***********************\n");
		printf("******1.查单词*********\n");
		printf("******2.历史记录*******\n");
		printf("******3.修改密码*******\n");
		printf("******4.返回上一级*****\n");
		printf("***********************\n");
		printf("请输入指令>>>");
		fflush(stdout);
		choose = getchar();
		while(getchar() != 10);
		switch(choose)
		{
		case '1':
			search(sfd, name);
			break;
		case '2':
			history(sfd, name);
			break;
		case '3':
			passwd_modify(sfd, name);
			break;
		case '4':
			back(sfd, name);
			back_flag = 1;
			break;
		default:
			printf("输入有误\n");
			printf("输入任意字符清屏: ");
			fflush(stdout);
			while(getchar()!=10);
			system("clear");
			continue;
		}
		if(back_flag)
		{
			*status = -1;
			break;
		}
	}

	return 0;
}

int do_register(int sfd)
{
	char buf[BUF_LEN] = "";
	char name[NAME_LEN] = "";
	char passwd[PASSWD_LEN] = "";


	printf("请输入用户名: ");
	fflush(stdout);
	scanf("%s", name);
	while(getchar()!=10);
	//printf("name=%s  __%d__\n", name, __LINE__);
	printf("请输入用户密码: ");
	fflush(stdout);
	scanf("%s", passwd);
	while(getchar()!=10);
	//  R+"name"+'#'+"passwd"
	sprintf(buf, "%c%s%c%s", 'R', name, '#', passwd);

	//发包到服务器
	send_to_server(sfd, buf);
	//接收ACK，解包
	bzero(buf, sizeof(buf));
	recv_from_server(sfd, buf);
	//printf("buf=%s  __%d__\n",buf, __LINE__);
	if('y' == buf[1])
	{
		//buf[1]='y',注册成功
		printf("%s注册成功\n\n", name);
	}else if('n' == buf[1])
	{
		//buf[1]='n',打印错误信息 buf+2
		err_msg((int)buf[2]);
	}
	return 0;
}


int login(int sfd, char *pname)
{
	char buf[BUF_LEN] = "";
	char name[NAME_LEN] = "";
	char passwd[PASSWD_LEN] = "";

	printf("请输入用户名: ");
	fflush(stdout);
	scanf("%s", name);
	while(getchar()!=10);
	printf("请输入用户密码: ");
	fflush(stdout);
	scanf("%s", passwd);
	while(getchar()!=10);
	//  L+"name"+#+"passwd"
	sprintf(buf, "%c%s%c%s", 'L', name, '#', passwd);

	//发包到服务器
	send_to_server(sfd, buf);
	//接收ACK，解包
	bzero(buf, sizeof(buf));
	recv_from_server(sfd, buf);
	//printf("buf=%s  __%d__\n",buf, __LINE__);
	if('y' == buf[1])
	{
		//buf[1]='y',登录成功
		strcpy(pname, name);
		printf("%s登录成功\n\n", name);
	}else if('n' == buf[1])
	{
		//buf[1]='n',打印错误信息 buf+2
		err_msg((int)buf[2]);
		return -1;
	}
	return 0;
}

int search(int sfd, char *pname)
{
	char buf[BUF_LEN] = "";
	char name[NAME_LEN] = "";
	char word[WORD_LEN] = "";
	char mean[MEAN_LEN] = "";

	strcpy(name, pname);
	while(1)
	{
		printf("\n请输入单词(输入'#'结束)：");
		fflush(stdout);
		fgets(word, sizeof(word), stdin);
		word[strlen(word)-1] = '\0';

		if(0 == strcmp(word, "#"))
		{
			break;
		}

		//printf("word=%s __%d__\n", word, __LINE__);
		//  S+"name"+#+"word"+#
		sprintf(buf, "%c%s%c%s%c", 'S', name, '#', word, '#');
		//发包到服务器
		send_to_server(sfd, buf);

		while(1)
		{
			//接收ACK，解包
			bzero(buf, sizeof(buf));
			recv_from_server(sfd, buf);
			if('y' == buf[1])
			{
				//buf[1]='y',打印word和mean
				sscanf(buf+2, "%[^#]#%[^#]", word, mean);
				printf("%s\t%s\n", word, mean);
			}
			else if('n' == buf[1])
			{
				//buf[1]='n',打印错误信息 buf+2
				err_msg((int)buf[2]);
				return -1;
			}
			else if('#' == buf[1])
			{
				break;
			}
		}
	}
	return 0;
}

int history(int sfd, char *pname)
{
	char buf[BUF_LEN] = "";
	char name[NAME_LEN] = "";
	char word[WORD_LEN] = "";
	char mean[MEAN_LEN] = "";
	char loc_time[TIME_LEN] = "";

	strcpy(name, pname);
	//  H+"name"
	sprintf(buf, "%c%s", 'H', name);
	//发包到服务器
	send_to_server(sfd, buf);

	while(1)
	{
		//接收ACK，解包
		bzero(buf, sizeof(buf));
		bzero(loc_time, sizeof(loc_time));
		bzero(name, sizeof(name));
		bzero(word, sizeof(word));
		bzero(mean, sizeof(mean));
		recv_from_server(sfd, buf);
		if('y' == buf[1])
		{
			//buf[1]='y',打印time、name、word、mean
			sscanf(buf+2, "%[^#]#%[^#]#%[^#]#%[^#]#", loc_time, name, word, mean);
			printf("%s\t%s\t%s\t%s\n", loc_time, name, word, mean);
		}else if('n' == buf[1])
		{
			//buf[1]='n',打印错误信息 buf+2
			err_msg((int)buf[2]);
			return -1;
		}
		else if('#' == buf[1])
		{
			break;
		}
	}
	return 0;
}

int passwd_modify(int sfd, char *pname)
{
	char buf[BUF_LEN] = "";
	char name[NAME_LEN] = "";
	char old_passwd[PASSWD_LEN] = "";
	char new_passwd[PASSWD_LEN] = "";
	char temp_passwd[PASSWD_LEN] = "";

	while(1)
	{
		printf("请输入旧密码: ");
		fflush(stdout);
		scanf("%s", old_passwd);
		while(getchar()!=10);

		printf("请输入新密码: ");
		fflush(stdout);
		scanf("%s", temp_passwd);
		while(getchar()!=10);

		printf("请再次输入新密码: ");
		fflush(stdout);
		scanf("%s", new_passwd);
		while(getchar()!=10);

		if(0 == strcmp(temp_passwd, new_passwd))
			break;
		else
			printf("两次密码不一致，请重新输入！\n");
	}

	
	strcpy(name, pname);
	//  P+"name"+#+"old_passwd"+#+"new_passwd"
	sprintf(buf, "%c%s%c%s%c%s", 'P', name, '#', old_passwd, '#', new_passwd);
	//发包到服务器
	send_to_server(sfd, buf);
	//接收ACK，解包
	bzero(buf, sizeof(buf));
	recv_from_server(sfd, buf);
	if('y' == buf[1])
	{
		//buf[1]='y',密码修改成功
		printf("密码修改成功\n");
	}
	else if('n' == buf[1])
	{
		//buf[1]='n',打印错误信息 buf+2
		err_msg((int)buf[2]);
		return -1;
	}
	return 0;
}

int back(int sfd, char *name)
{
	char buf[BUF_LEN] = "";
	//char name[NAME_LEN] = "";

	//  B+"name"
	sprintf(buf, "%c%s", 'B', name);
	//发包到服务器
	send_to_server(sfd, buf);
	return 0;
}


int recv_from_server(int sfd, char *buf)
{
	ssize_t res;
	//接受ACK
	res = recv(sfd, buf, BUF_LEN, 0);
	if(res < 0)
		ERR_MSG("recv", -1);

	return 0;
}

int send_to_server(int sfd, char *buf)
{
	ssize_t res;
	//发包到服务器
	res = send(sfd, buf, BUF_LEN, 0);
	//fprintf(stdout, "buf=%s  __%d__\n", buf, __LINE__);
	if(res < 0)
		ERR_MSG("send", -1);

	return 0;
}

int err_msg(int err_code)
{
	switch(err_code)
	{
	case 1:
		break;
	case 2:
		printf("用户名已存在\n");
		break;
	case 3:
		printf("重复登录\n");
		break;
	case 4:
		printf("查无此人\n");
		break;
	case 5:
		printf("密码输入有误\n");
		break;
	case 6:
		printf("未查到结果\n");
		break;
	case 7:
		printf("无历史记录\n");
		break;
	}
	return 0;
}
