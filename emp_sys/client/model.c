#include "./model.h"

int admin_flag = 0;
int loop_main(int sfd)
{
	int choose = 0;
	int exit_flag = 0;
	int status = -1;
	char id[ID_LEN] = "";

	while(1)
	{
		printf("************************\n");
		printf("********1.注册**********\n");
		printf("********2.员工登录******\n");
		printf("********3.管理员登录****\n");
		printf("********4.退出**********\n");
		printf("************************\n");
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
			status = login(sfd, id);
			break;
		case '3':
			admin_flag = 1;
			status = login(sfd, id);
			break;	
		case '4':
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
		if((0 == status) && (0 == admin_flag))//普通用户登陆成功
			loop_usr(sfd, &status, id);
		else if((0 == status) && (1 == admin_flag))//管理员登陆成功
			loop_admin(sfd, &status, id);
	}

	return 0;
}

int loop_usr(int sfd, int *status, char *id)
{
	int choose = 0;
	int back_flag = 0;

	while(1)
	{
		printf("************************************\n");
		printf("*********** ###员工模式### **********\n");
		printf("*************1.查询信息**************\n");
		printf("*************2.修改信息**************\n");
		printf("*************3.修改密码**************\n");
		printf("*************4.返回上一级************\n");
		printf("************************************\n");
		printf("请输入指令>>>");
		fflush(stdout);
		choose = getchar();
		while(getchar() != 10);
		switch(choose)
		{
		case '1':
			search(sfd, id);
			break;			
		case '2':
			info_mod_usr(sfd, id);
			break;
		case '3':
			passwd_modify(sfd, id);
			break;
		case '4':
			back(sfd, id);
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

int loop_admin(int sfd, int *status, char *id)
{
	int choose = 0;
	int back_flag = 0;

	while(1)
	{
		printf("**************************************\n");
		printf("********** ###管理员模式### ***********\n");
		printf("**************1.增加员工***************\n");
		printf("**************2.删除员工***************\n");
		printf("**************3.修改员工信息***********\n");
		printf("**************4.查询员工信息***********\n");
		printf("**************5.修改管理员密码*********\n");
		printf("**************6.返回上一级*************\n");
		printf("**************************************\n");
		printf("请输入指令>>>");
		fflush(stdout);
		choose = getchar();
		while(getchar() != 10);
		switch(choose)
		{
		case '1':
			add_employees(sfd);
			break;			
		case '2':
			del_employees(sfd);
			break;
		case '3':
			info_mod_admin(sfd);
			break;
		case '4':
			search_employees(sfd);
			break;
		case '5':
			passwd_modify(sfd, id);
			break;
		case '6':
			back(sfd, id);
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
	char age[AGE_LEN] = "";
	char sex[SEX_LEN] = "";
	char addr[ADDR_LEN] = "";
	char phone[PHONE_LEN] = "";
	char passwd[PASSWD_LEN] = "";

	while(1)
	{
		printf("请输入姓名: ");
		fflush(stdout);
		scanf("%s", name);
		while(getchar()!=10);
		if(0 == strcmp(name, "Admin"))
		{
			printf("输入不合法，请重新输入\n");
			bzero(name, sizeof(name));
		}else
			break;
	}
	
	printf("请输入年龄: ");
	fflush(stdout);
	scanf("%s", age);
	while(getchar()!=10);
	printf("请输入性别: ");
	fflush(stdout);
	scanf("%s", sex);
	while(getchar()!=10);
	printf("请输入手机号: ");
	fflush(stdout);
	scanf("%s", phone);
	while(getchar()!=10);
	printf("请输入地址: ");
	fflush(stdout);
	scanf("%s", addr);
	while(getchar()!=10);
	printf("请输入密码: ");
	fflush(stdout);
	scanf("%s", passwd);
	while(getchar()!=10);

	//  R+"name"+'#'+"age"+'#'+"sex"+'#'+"phone"+'#'+"addr"+'#'+"passwd"+'#'
	sprintf(buf, "%c%s%c%s%c%s%c%s%c%s%c%s%c", 'R', name, '#', age, '#', sex, '#', phone, '#', addr, '#', passwd, '#');
	
	//发包到服务器
	send_to_server(sfd, buf);

	//接收ACK，解包
	bzero(buf, sizeof(buf));
	recv_from_server(sfd, buf);
	//printf("buf=%s  __%d__\n",buf, __LINE__);
	if('y' == buf[1])
	{
		//buf[1]='y',注册成功
		printf("工号: %d, 员工注册成功\n\n", (int)buf[3]);
	}else if('n' == buf[1])
	{
		//buf[1]='n',打印错误信息 buf+2
		err_msg((int)buf[2]);
	}
	return 0;
}

int add_employees(int sfd)
{
	int i = 1;
	char ch = '0';
	
	while(1)
	{
		printf("开始录入第%d个员工信息>>>\n", i++);
		if(do_register(sfd) == 0)
		{
			printf("录入成功!请问是否需要继续录入?(y/n):");
			fflush(stdout);
			ch = getchar();
			while(getchar()!=10);
		}
		if(ch == 'n')
		{
			printf("全部数据录入完毕!\n");
			break;
		}
	}
	return 0;
}

int del_employees(int sfd)
{
	int ret = 0;
	char buf[BUF_LEN] = "";
	char id[ID_LEN] = "";
	char ch = '0';

	while(1)
	{
		printf("\n请输入要删除的工号(>0): ");
		fflush(stdout);
		fgets(id, sizeof(id), stdin);
		id[strlen(id)-1] = '\0';
		if(0 == strcmp(id, "0"))
		{
			printf("输入不合法，请重新输入\n");
			bzero(id, sizeof(id));
		}else
			break;
	}

	ret = search(sfd, id);
	if(ret < 0)
	{
		return -1;
	}

	printf("请确认是否删除?(y/n): ");
	fflush(stdout);
	ch = getchar();
	while(getchar()!=10);
	if(ch == 'n')
	{
		printf("已取消本次删除操作!\n");
		return -1;
	}

	//  D+"id"+#
	sprintf(buf, "%c%s%c", 'D', id, '#');
	//发包到服务器
	send_to_server(sfd, buf);

	//接收ACK，解包
	bzero(buf, sizeof(buf));
	recv_from_server(sfd, buf);
	if('n' == buf[1])
	{
		//buf[1]='n',打印错误信息 
		printf("工号: %s, 删除失败\n", id);
		return -1;
	}
	printf("工号: %s, 删除成功\n", id);

	return 0;
}

int login(int sfd, char *pid)
{
	char buf[BUF_LEN] = "";
	char id[ID_LEN] = "";
	char passwd[PASSWD_LEN] = "";

	if(admin_flag == 0)//普通用户
	{
		while(1)
		{
			printf("请输入工号(>0): ");
			fflush(stdout);
			scanf("%s", id);
			while(getchar()!=10);
			if((0 == strcmp(id, "0")) || (id[0] == '-'))
			{
				printf("输入不合法，请重新输入\n");
				bzero(id, sizeof(id));
			}else
				break;
		}
	}else//管理员
		strcpy(id, "0");

	printf("请输入密码: ");
	fflush(stdout);
	scanf("%s", passwd);
	while(getchar()!=10);
	//  L+"id"+#+"passwd"
	sprintf(buf, "%c%s%c%s", 'L', id, '#', passwd);

	//发包到服务器
	send_to_server(sfd, buf);
	//接收ACK，解包
	bzero(buf, sizeof(buf));
	recv_from_server(sfd, buf);
	//printf("buf=%s  __%d__\n",buf, __LINE__);
	if('y' == buf[1])
	{
		//buf[1]='y',登录成功
		if(admin_flag == 0)
		{
			printf("工号: %s, 员工登录成功\n\n", id);
		}else
			printf("管理员登录成功\n\n");
		strcpy(pid, id);//保存登陆成功的id
	}else if('n' == buf[1])
	{
		//buf[1]='n',打印错误信息 buf+2
		err_msg((int)buf[2]);
		admin_flag = 0;//登陆失败，管理员登录标志位置0
		return -1;
	}
	return 0;
}

int search(int sfd, char *pid)
{
	char buf[BUF_LEN] = "";
	char id[ID_LEN] = "";
	char name[NAME_LEN] = "";
	char age[AGE_LEN] = "";
	char sex[SEX_LEN] = "";
	char addr[ADDR_LEN] = "";
	char phone[PHONE_LEN] = "";
	char wage[WAGE_LEN] = "";

	//  S+"id"+#
	sprintf(buf, "%c%s%c%c", 'S', pid, '#', '%');// % 表示为普通用户
	//发包到服务器
	send_to_server(sfd, buf);
	
	while(1)
	{
		//接收ACK，解包
		bzero(buf, sizeof(buf));
		recv_from_server(sfd, buf);
		if('y' == buf[1])
		{
			//buf[1]='y',打印查询到的信息
			sscanf(buf+2, "%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]",id,name,age,sex,phone,addr,wage);
			printf("工号:%s\t  姓名:%s\t  年龄(岁):%s\t  性别:%s\t\n电话:%s\t地址:%s\t薪资(元):%s\t\n", id,name,age,sex,phone,addr,wage);
		}else if('n' == buf[1])
		{
			//buf[1]='n',打印错误信息 buf+2
			err_msg((int)buf[2]);
			return -1;
		}else if('#' == buf[1])
		{
			break;
		}
	}

	return 0;
}

int search_employees(int sfd)
{
	char buf[BUF_LEN] = "";
	char id[ID_LEN] = "";
	char name[NAME_LEN] = "";
	char age[AGE_LEN] = "";
	char sex[SEX_LEN] = "";
	char addr[ADDR_LEN] = "";
	char phone[PHONE_LEN] = "";
	char wage[WAGE_LEN] = "";
	char passwd[PASSWD_LEN] = "";
	char status[STATUS_LEN] = "";

	while(1)
	{
		printf("\n请输入工号(输入一个'#'结束查询): ");
		fflush(stdout);
		fgets(id, sizeof(id), stdin);
		id[strlen(id)-1] = '\0';

		//if(0 == strcmp(id, "#"))
		if(id[0] == '#')
		{
			break;
		}

		//printf("word=%s __%d__\n", word, __LINE__);
		//  S+"id"+#
		sprintf(buf, "%c%s%c%c", 'S', id, '#', '$');// $ 表示为管理员
		//发包到服务器
		send_to_server(sfd, buf);

		while(1)
		{
			//接收ACK，解包
			bzero(buf, sizeof(buf));
			recv_from_server(sfd, buf);
			if('y' == buf[1])
			{
				//buf[1]='y',打印查询到的信息
				sscanf(buf+2, "%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]",\
					id,name,age,sex,phone,addr,wage,passwd,status);
				printf("工号:%s\t姓名:%s\t年龄(岁):%s\t性别:%s\t电话:%s\t\n地址:%s\t薪资(元):%s\t密码:%s\t状态:%s\t\n", \
					id,name,age,sex,phone,addr,wage,passwd,status);
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

#if 0
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
#endif

int info_mod_usr(int sfd, char *id)
{
	char buf[BUF_LEN] = "";
	char str[ADDR_LEN] = "";
	char choose = '0';
	
	printf("**************************************\n");
	printf("修改指令:\n");
	printf("1-姓名\t2-性别\t3-手机号\t4-地址\t\n");
	printf("**************************************\n");
	printf("请输入指令>>>");
	fflush(stdout);
	choose = getchar();
	while(getchar() != 10);
	switch(choose)
	{			
		case '1':
			choose = '2'; //对应服务器指令
			printf("请输入姓名: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;	
		case '2':
			choose = '4';
			printf("请输入性别: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;
		case '3':
			choose = '5';
			printf("请输入手机号: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;
		case '4':
			choose = '6';
			printf("请输入地址: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;
		default:
			printf("输入有误\n");
			printf("输入任意字符清屏: ");
			fflush(stdout);
			while(getchar()!=10);
			system("clear");
	}

	//  I+"id"+#+choose+#str+#
	sprintf(buf, "%c%s%c%c%c%s%c", \
		'I', id, '#', choose, '#', str, '#');
	//发包到服务器
	send_to_server(sfd, buf);

	//接收ACK，解包
	bzero(buf, sizeof(buf));
	recv_from_server(sfd, buf);
	if('n' == buf[1])
	{
		//buf[1]='n',打印错误信息 
		printf("修改信息失败\n");
		return -1;
	}
	printf("修改信息成功\n\n");
	search(sfd, id);

	return 0;
}

int info_mod_admin(int sfd)
{
	int ret = 0;
	char buf[BUF_LEN] = "";
	char id[ID_LEN] = "";
	char new_id[ID_LEN] = "*";
	char str[ADDR_LEN] = "";
	char choose = '0';

	while(1)
	{
		printf("\n请输入要修改信息的工号(>0): ");
		fflush(stdout);
		fgets(id, sizeof(id), stdin);
		id[strlen(id)-1] = '\0';
		if(0 == strcmp(id, "0"))
		{
			printf("输入不合法，请重新输入\n");
			bzero(id, sizeof(id));
		}else
			break;
	}

	printf("\n工号: %s, 当前信息如下: \n", id);
	ret = search(sfd, id);
	if(ret < 0)
	{
		return -1;
	}
	
	printf("**************************************\n");
	printf("修改指令:\n");
	printf("1-工号\t2-姓名\t3-年龄\t4-性别\t\n5-手机号\t6-地址\t7-薪资\t8-密码\t\n");
	printf("**************************************\n");
	printf("请输入指令>>>");
	fflush(stdout);
	choose = getchar();
	while(getchar() != 10);
	switch(choose)
	{
		case '1':
			printf("请输入工号: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			strcpy(new_id, str);
			break;			
		case '2':
			printf("请输入姓名: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;	
		case '3':
			printf("请输入年龄: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;
		case '4':
			printf("请输入性别: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;
		case '5':
			printf("请输入手机号: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;
		case '6':
			printf("请输入地址: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;
		case '7':
			printf("请输入薪资: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;
		case '8':
			printf("请输入密码: ");
			fflush(stdout);
			scanf("%s", str);
			while(getchar()!=10);
			break;
		default:
			printf("输入有误\n");
			printf("输入任意字符清屏: ");
			fflush(stdout);
			while(getchar()!=10);
			system("clear");
	}

	//  I+"id"+#+choose+#str+#
	sprintf(buf, "%c%s%c%c%c%s%c", \
		'I', id, '#', choose, '#', str, '#');
	//发包到服务器
	send_to_server(sfd, buf);

	//接收ACK，解包
	bzero(buf, sizeof(buf));
	recv_from_server(sfd, buf);
	if('n' == buf[1])
	{
		//buf[1]='n',打印错误信息 
		printf("修改信息失败\n");
		return -1;
	}
	printf("修改信息成功\n\n");
	
	if(0 == strcmp(new_id, str))
		search(sfd, new_id);
	else
		search(sfd, id);

	return 0;
}

int passwd_modify(int sfd, char *pid)
{
	char buf[BUF_LEN] = "";
	char id[ID_LEN] = "";
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

	
	strcpy(id, pid);
	//  P+"id"+#+"old_passwd"+#+"new_passwd"
	sprintf(buf, "%c%s%c%s%c%s", 'P', id, '#', old_passwd, '#', new_passwd);
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

int back(int sfd, char *id)
{
	char buf[BUF_LEN] = "";
	//char name[NAME_LEN] = "";

	//  B+"id"
	sprintf(buf, "%c%s", 'B', id);
	//发包到服务器
	send_to_server(sfd, buf);
	admin_flag = 0;
	
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
		printf("用户已存在\n");
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
