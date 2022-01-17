#include "controller.h"

int main(int argc,char *argv[])
{
    //int ret = 0;
    struct DOU_LINKLIST *phead = create_doubly_linklist();    
    
    mainloop(phead);
 
    destroy_doubly_linklist(phead);
    
    return 0;
}

int mainloop(struct DOU_LINKLIST *phead)
{
    int op = 0;
	int ret = 0;
    int exitflag = 0;//退出系统标记
    int loginflag = 0;//记录登录状态
    //int ret = 0;
	int id = 0;
	int login_id = 0;
	char name[NAME_LEN] = "\0";
	char passwd[PASSWD_LEN] = "\0";
	struct PERSON_DATA newdata = {"###","###",0,0,0,0,'#'};
	struct PERSON_DATA *pdata = NULL;
	struct LIST_NODE *pnewnode = NULL;
	struct LIST_NODE *pnode = NULL;
    struct LIST_NODE *p_stu = NULL;//登录成功的学生节点
    
    while(1)
    {
        op = display_main_ui();
        switch(op)
        {
            case 1:
				if(loginflag == 0)
				{
                	p_stu = login(phead);
					if(p_stu != NULL)
					login_id = p_stu->pdata->id;//记录登录成功的id(sort之后pdata指向发生改变)
						loginflag = 1;
				}
                break;
            case 2:
                if(loginflag == 0)
					break;
				p_stu = search_list_node_by_id(phead, &login_id);//找到ID所在节点
				if(p_stu == NULL)
					break;
				ret = modify_passwd(p_stu);
				if(ret == 0)
				{
					printf("***********Please login again********\n");
					loginflag = 0;
                	p_stu = login(phead);
					if(p_stu != NULL)
						loginflag = 1;
				}
                break;
            case 3:
				InputUserName(name);
				strcpy(newdata.name, name);
				InputPasswd(passwd);
				strcpy(newdata.passwd, passwd);
				InputID(&newdata.id);
				InputSex(&newdata.sex);
				InputMathScore(&newdata.math_score);
				InputChineseScore(&newdata.chn_score);
				InputEnglishScore(&newdata.eng_score);

				pnewnode = create_list_node();
				pdata = create_person_data(&newdata);
				insert_list_node_at_last(phead,pnewnode,pdata);
                break;
			case 4:
                if(loginflag == 0)
					break;
				p_stu = search_list_node_by_id(phead, &login_id);//找到ID所在节点
				if(p_stu == NULL)
					break;
				print_now_list_node(p_stu);
				break;
			case 5:
                if(loginflag == 0)
					break;
				InputID(&id);
				pnode = search_list_node_by_id(phead,&id);
				remove_list_node(phead, pnode);
				ret = destroy_list_node(pnode);
				if(ret == 0)
				{
					p_stu = NULL;
				}
				break;
			case 6:
                if(loginflag == 0)
					break;
				InputID(&id);
				pnode = search_list_node_by_id(phead,&id);
				print_now_list_node(pnode);
				break;
			case 7:
                if(loginflag == 0)
					break;

				p_stu = search_list_node_by_id(phead, &login_id);//找到ID所在节点
				if(p_stu == NULL)
					break;
				InputUserName(name);
				strcpy(p_stu->pdata->name, name);
				InputSex(&p_stu->pdata->sex);
				InputMathScore(&p_stu->pdata->math_score);
				InputChineseScore(&p_stu->pdata->chn_score);
				InputEnglishScore(&p_stu->pdata->eng_score);
				break;
			case 8:
                if(loginflag == 0)
					break;
				PrintAllStudentByID(phead);
				break;
			case 9:
                if(loginflag == 0)
					break;
				PrintAllStudentByMathScore(phead);
				break;
			case 10:
                if(loginflag == 0)
					break;
				PrintAllStudentByChineseScore(phead);
				break;
			case 11:
                if(loginflag == 0)
					break;
				PrintAllStudentByEnglishScore(phead);
				break;
			case 12:
                if(loginflag == 0)
					break;
				PrintAllStudentByTotalScore(phead);
				break;
            case 0:
                exitflag = 1;
                break;
			default:;
        }
        if(exitflag)
        {
            break;
        }
    }
    
    return 0;
}
