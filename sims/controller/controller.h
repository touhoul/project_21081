#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doubly_linklist.h"
#include "viewer.h"

struct LIST_NODE *login(struct DOU_LINKLIST *phead);
int modify_passwd(struct LIST_NODE *p_stu);
int mainloop(struct DOU_LINKLIST *phead);


/*
int sort_list_node_by_id(struct DOU_LINKLIST *phead);//从低到高
int sort_list_node_by_math_score(struct DOU_LINKLIST *phead);//从高到低
int sort_list_node_by_chinese_score(struct DOU_LINKLIST *phead);//从高到低
int sort_list_node_by_english_score(struct DOU_LINKLIST *phead);//从高到低
int sort_list_node_by_total_score(struct DOU_LINKLIST *phead);//从高到低
*/

#endif

