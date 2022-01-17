#ifndef _DOUBLY_LINK_LIST_
#define _DOUBLY_LINK_LIST_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 20
#define PASSWD_LEN 20

struct PERSON_DATA
{
	char name[NAME_LEN];//人名不支持空白字符
	char passwd[PASSWD_LEN];//密码不支持空白字符
	int id;
	float math_score;
	float chn_score;
	float eng_score;
	char sex;
};

struct LIST_NODE
{
	struct LIST_NODE *pprev;
	struct LIST_NODE *pnext;
	struct PERSON_DATA *pdata;
};

struct DOU_LINKLIST
{
	struct LIST_NODE *pfirst;
	int len;
};


struct DOU_LINKLIST* create_doubly_linklist();
int clear_all_linklist_node(struct DOU_LINKLIST *phead);
int destroy_doubly_linklist(struct DOU_LINKLIST *phead);

struct LIST_NODE* create_list_node();
int destroy_list_node(struct LIST_NODE *pnode);

struct PERSON_DATA* create_person_data(struct PERSON_DATA *pdata);
int destroy_person_data(struct PERSON_DATA *pdata);

int insert_list_node_at_first(struct DOU_LINKLIST *phead, struct LIST_NODE *pnewnode, struct PERSON_DATA *pdata);
int insert_list_node_at_last(struct DOU_LINKLIST *phead, struct LIST_NODE *pnewnode, struct PERSON_DATA *pdata);
int insert_list_node_after_node(struct DOU_LINKLIST *phead, struct LIST_NODE *pnode, struct LIST_NODE *pnewnode, struct PERSON_DATA *pdata);

struct LIST_NODE* remove_list_node_at_first(struct DOU_LINKLIST *phead);
struct LIST_NODE* remove_list_node_at_last(struct DOU_LINKLIST *phead);
struct LIST_NODE* remove_list_node(struct DOU_LINKLIST *phead, struct LIST_NODE *pnode);

struct LIST_NODE* search_list_node_by_name(struct DOU_LINKLIST *phead, char *pname);
struct LIST_NODE* search_list_node_by_passwd(struct DOU_LINKLIST *phead, char *ppasswd);
struct LIST_NODE* search_list_node_by_id(struct DOU_LINKLIST *phead, int *pid);

int modify_list_node(struct LIST_NODE *pnode, struct PERSON_DATA *pdata);

int sort_list_node_by_id(struct DOU_LINKLIST *phead);//从低到高
int sort_list_node_by_math_score(struct DOU_LINKLIST *phead);//从高到低
int sort_list_node_by_chinese_score(struct DOU_LINKLIST *phead);//从高到低
int sort_list_node_by_english_score(struct DOU_LINKLIST *phead);//从高到低
int sort_list_node_by_total_score(struct DOU_LINKLIST *phead);//从高到低

int print_doubly_linklist(struct DOU_LINKLIST *phead);
int print_now_list_node(struct LIST_NODE *pnode);

#endif

