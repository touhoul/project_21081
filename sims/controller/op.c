#include "controller.h"



/*login函数定义*/
struct LIST_NODE* login(struct DOU_LINKLIST *phead)
{
	char name[NAME_LEN] = "\0";
	char passwd[PASSWD_LEN] = "\0";
	struct LIST_NODE *p_stu = NULL;

    //调用输入名字的函数
	InputUserName(name);

    //查找节点
	p_stu = search_list_node_by_name(phead,name);
	if(NULL == p_stu)
	{
		PrintErrMsg(name);
		return NULL;
	}

    //调用输入密码的函数
	InputPasswd(passwd);
	if( strcmp(p_stu->pdata->passwd, passwd) == 0)
	{
		return p_stu;
	}else
	{
		PrintErrMsg(passwd);
		return NULL;
	}   
}

//其它功能函数

int modify_passwd(struct LIST_NODE *p_stu)
{
	char passwd1[PASSWD_LEN] = "\0";
	char passwd2[PASSWD_LEN] = "\0";
	InputPasswd(passwd1);	
	InputPasswdAgain(passwd2);
	if(strcmp(passwd1,passwd2) == 0)
	{
		strcpy(p_stu->pdata->passwd, passwd2);
		return 0;
	}else
	{
		PrintErrMsg(passwd2);
		return -1;
	}
}
