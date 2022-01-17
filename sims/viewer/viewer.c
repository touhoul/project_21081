#include "viewer.h"

int display_main_ui()
{
    int op = -1;
/*    
1.  修改自身登录密码
2.  添加新学生
3.  查阅自身信息
4.  删除学生（输入学号）
5.  查阅指定学生信息（输入学号）（密码不打印）
6.  修改自己的信息（姓名、性别、三门功课成绩）
7.  按学号从低到高查看所有学生信息（密码不打印）
8.  按数学成绩从高到低查看所有学生信息（密码不打印）
9.  按语文成绩从高到低查看所有学生信息（密码不打印）
10.  按英语成绩从高到低查看所有学生信息（密码不打印）
11.  按总分从高到低查看所有学生信息（密码不打印）
*/
    printf("**************************************\n");
    printf("Please select your operation:\n");
    printf("1.  Login User\n");
    printf("2.  Update Password\n");
    printf("3.  Add a new student\n");
    printf("4.  Query your information\n");
    printf("5.  Delete student(input id)\n");
    printf("6.  Search student(input id)\n");
    printf("7.  Modify your information(name/sex/score)\n");
    printf("8.  View all student information by id from high to low\n");
    printf("9.  View all student information by Math-score from high to low\n");
    printf("10. View all student information by Chinese-score from high to low\n");
    printf("11. View all student information by English-score from high to low\n");
    printf("12. View all student information by total-score from high to low\n");
    printf("0.  Exit\n");
    printf("**************************************\n");
    scanf("%d",&op);
    
    while(getchar()!='\n')
    {        
    }
    
    return op;
}


/*InputUserName函数定义*/
int InputUserName(char *pname)
{
    printf(">>>Please input your name:\n");
    scanf("%s",pname);
    
    while(getchar()!='\n')
    {        
    }
    return 0;
}

/*InputPasswd函数定义*/
int InputPasswd(char *ppasswd)
{
    printf(">>>Please input your Password:\n");
    scanf("%s",ppasswd);
    
    while(getchar()!='\n')
    {        
    }
    return 0;
}

/*InputPasswdAgain函数定义*/
int InputPasswdAgain(char *ppasswd)
{
    printf(">>>Please input your Password again:\n");
    scanf("%s",ppasswd);
    
    while(getchar()!='\n')
    {        
    }
    return 0;
}

/*InputID函数定义*/
int InputID(int *pnewid)
{
    printf(">>>Please input your ID:\n");
    scanf("%d",pnewid);
    
    while(getchar()!='\n')
    {        
    }
    return 0;
}

int InputSex(char *psex)
{
    printf(">>>Please input your Sex:\n");
    scanf("%c",psex);
    
    while(getchar()!='\n')
    {        
    }
    return 0;
}

int InputMathScore(float *pmath)
{
    printf(">>>Please input your math score:\n");
    scanf("%f",pmath);
    
    while(getchar()!='\n')
    {        
    }
    return 0;
}

int InputChineseScore(float *pchn)
{
    printf(">>>Please input your chinese score:\n");
    scanf("%f",pchn);
    
    while(getchar()!='\n')
    {        
    }
    return 0;
}

int InputEnglishScore(float *peng)
{
    printf(">>>Please input your english score:\n");
    scanf("%f",peng);
    
    while(getchar()!='\n')
    {        
    }
    return 0;
}

void PrintErrMsg(char *pstr)
{
	//printf("");
    printf("%s is error\n",pstr);
}

/*
int print_doubly_linklist(struct DOU_LINKLIST *phead)
{
    struct LIST_NODE *pnode = phead->pfirst;
    float total = 0;
    if(phead->pfirst == NULL)
    {   
        printf("doubly_linklist is empty\n");
        return 1;
    }   
    while(pnode != NULL)
    {
        total = pnode->pdata->math_score + pnode->pdata->chn_score + pnode->pdata->eng_score;
        printf("Name:%s\tID:%d\tSex:%c\tMath:%.1f\tChinese:%1.f\tEnglish:%1.f\tTotal-Score:%1.f\t\n",
                pnode->pdata->name, pnode->pdata->id, pnode->pdata->sex, pnode->pdata->math_score, pnode->pdata->chn_score, pnode->pdata->eng_score, total);
        pnode = pnode->pnext;
    }   
    printf("\n");
    return 0;                                                                                                                                                               
}
*/


/*PrintAllStudentByID函数定义*/
int PrintAllStudentByID(struct DOU_LINKLIST *phead)
{
	printf(">>>>>View all student information by student id from low to high:\n");
	sort_list_node_by_id(phead);
	print_doubly_linklist(phead);
	return 0;
}

/*PrintAllStudentByMathScore函数定义*/
int PrintAllStudentByMathScore(struct DOU_LINKLIST *phead)
{
	printf(">>>>>View all student information by math score from high to low:\n");
	sort_list_node_by_math_score(phead);
	print_doubly_linklist(phead);
	return 0;
}

/*PrintAllStudentByChineseScore函数定义*/
int PrintAllStudentByChineseScore(struct DOU_LINKLIST *phead)
{
	printf(">>>>>View all student information by chinese score from high to low:\n");
	sort_list_node_by_chinese_score(phead);
	print_doubly_linklist(phead);
	return 0;
}

/*PrintAllStudentByEnglishScore函数定义*/
int PrintAllStudentByEnglishScore(struct DOU_LINKLIST *phead)
{
	printf(">>>>>View all student information by english score from high to low:\n");
	sort_list_node_by_english_score(phead);
	print_doubly_linklist(phead);
	return 0;
}

/*PrintAllStudentByTotalScore函数定义*/
int PrintAllStudentByTotalScore(struct DOU_LINKLIST *phead)
{
	printf(">>>>>View all student information by total score from high to low:\n");
	sort_list_node_by_total_score(phead);
	print_doubly_linklist(phead);
	return 0;
}
