#include "doubly_linklist.h"


struct DOU_LINKLIST* create_doubly_linklist()
{
	struct DOU_LINKLIST *phead = NULL;
	phead = (struct DOU_LINKLIST*)malloc(sizeof(struct DOU_LINKLIST));
	if(phead == NULL)
	{
		//printf("malloc failed for phead\n");
		return NULL;
	}
	memset(phead, 0, sizeof(struct DOU_LINKLIST));
	return phead;
}

int clear_all_linklist_node(struct DOU_LINKLIST *phead)
{
	struct LIST_NODE *pnode = phead->pfirst;
	struct LIST_NODE *pnext = NULL;
	while(pnode != NULL)
	{
		pnext = pnode->pnext;
		pnode->pprev = NULL;
		destroy_list_node(pnode);
		pnode = pnext;
	}
	phead->pfirst = NULL;
	return 0;
}

int destroy_doubly_linklist(struct DOU_LINKLIST *phead)
{
	clear_all_linklist_node(phead);
	free(phead);
	phead = NULL;
	return 0;
}

struct LIST_NODE* create_list_node()
{
	struct LIST_NODE *pnode = NULL;
	pnode = (struct LIST_NODE*)malloc(sizeof(struct LIST_NODE));
	if(pnode == NULL)
	{
		//printf("malloc failed for pnode\n");
		return NULL;
	}
	memset(pnode, 0, sizeof(struct LIST_NODE));
	return pnode;
}

int destroy_list_node(struct LIST_NODE *pnode)
{
	if(pnode != NULL)
	{
		destroy_person_data(pnode->pdata);
		free(pnode);
		pnode = NULL;
		return 0;
	}else
		return 1;
}

struct PERSON_DATA* create_person_data(struct PERSON_DATA *pdata)
{
	struct PERSON_DATA *pnewdata = NULL;
	pnewdata = (struct PERSON_DATA*)malloc(sizeof(struct PERSON_DATA));
	if(pnewdata == NULL)
	{
		//printf("malloc failed for pnewdata\n");
		return NULL;
	}
	memset(pnewdata, 0, sizeof(struct PERSON_DATA));
	memcpy(pnewdata, pdata, sizeof(struct PERSON_DATA));
	return pnewdata;
}

int destroy_person_data(struct PERSON_DATA *pdata)
{
	if(pdata != NULL)
	{
		free(pdata);
		pdata = NULL;
		return 0;
	}else
		return 1;
}

int insert_list_node_at_first(struct DOU_LINKLIST *phead, struct LIST_NODE *pnewnode, struct PERSON_DATA *pdata)
{
	struct PERSON_DATA *pnewdata = create_person_data(pdata);
	pnewnode->pdata = pnewdata;

	pnewnode->pnext = phead->pfirst;

	if(phead->pfirst != NULL)
	{
		phead->pfirst->pprev = pnewnode;
	}
	phead->pfirst = pnewnode;
	pnewnode->pprev = NULL;
	phead->len++;
	return 0;
}

int insert_list_node_at_last(struct DOU_LINKLIST *phead, struct LIST_NODE *pnewnode, struct PERSON_DATA *pdata)
{
	struct LIST_NODE *plast = NULL;
	struct PERSON_DATA *pnewdata = create_person_data(pdata);
	pnewnode->pdata = pnewdata;

	plast = phead->pfirst;
	while(plast != NULL)
	{
		if(plast->pnext == NULL)
			break;
		plast = plast->pnext;
	}
	if(plast != NULL)
	{
		plast->pnext = pnewnode;
		pnewnode->pprev = plast;
	}else
	{
		phead->pfirst = pnewnode;
		pnewnode->pprev = NULL;
	}
	pnewnode->pnext = NULL;
	phead->len++;
	return 0;
}

static int is_in_doubly_linklist(struct DOU_LINKLIST *phead, struct LIST_NODE *pnode)
{
	struct LIST_NODE *ptemp = NULL;
	ptemp = phead->pfirst;
	
	if(pnode == NULL)
	{
		//printf("pnode is NULL\n");
		return 0;
	}
	while(ptemp != NULL)
	{
		if(ptemp == pnode)
			break;
		ptemp = ptemp->pnext;
	}
	if(ptemp != NULL)
	{
		return 1;
	}else
	{
		//printf("linklist is empty\n");
		return 0;
	}
}

int insert_list_node_after_node(struct DOU_LINKLIST *phead, struct LIST_NODE *pnode, struct LIST_NODE *pnewnode, struct PERSON_DATA *pdata)
{
	struct PERSON_DATA *pnewdata = create_person_data(pdata);
	pnewnode->pdata = pnewdata;

	if(is_in_doubly_linklist(phead, pnode))
	{
		pnewnode->pnext = pnode->pnext;
		if(pnode->pnext != NULL)
		{
			pnode->pnext->pprev = pnewnode;
		}
		pnode->pnext = pnewnode;
		pnewnode->pprev = pnode;
	}else
	{
		//printf("pnode is not in linklist\n");
		return 1;
	}
	phead->len++;
	return 0;
}

struct LIST_NODE* remove_list_node_at_first(struct DOU_LINKLIST *phead)
{
	struct LIST_NODE *pnode = NULL;
	pnode = phead->pfirst;
	if(pnode == NULL)
	{
		//printf("doubly_linklist is empty\n");
		return NULL;
	}

	phead->pfirst = pnode->pnext;
	if(pnode->pnext != NULL)//至少2个节点
	{
		pnode->pnext->pprev = NULL;
	}
	pnode->pnext = NULL;
	phead->len--;
	return pnode;
}

struct LIST_NODE* remove_list_node_at_last(struct DOU_LINKLIST *phead)
{
	struct LIST_NODE *plast = NULL;
	plast = phead->pfirst;
	if(plast == NULL)
	{
		//printf("doubly_linklist is empty\n");
		return NULL;
	}
	while(plast != NULL)
	{
		if(plast->pnext == NULL)
			break;
		plast = plast->pnext;
	}
	if(plast->pprev != NULL)
	{
		plast->pprev->pnext = NULL;
		plast->pprev = NULL;
	}else
		phead->pfirst = NULL;

	phead->len--;
	return plast;
}


struct LIST_NODE* remove_list_node(struct DOU_LINKLIST *phead, struct LIST_NODE *pnode)
{
	if(is_in_doubly_linklist(phead, pnode))
	{
		if(pnode->pprev != NULL)
			pnode->pprev->pnext = pnode->pnext;
		else
			phead->pfirst = pnode->pprev;

		if(pnode->pnext != NULL)
			pnode->pnext->pprev = pnode->pprev;

		pnode->pprev = NULL;
		pnode->pnext = NULL;
	}else
	{
		//printf("pnode is not in linklist\n");
		return NULL;
	}
	phead->len--;
	return pnode;
}

struct LIST_NODE* search_list_node_by_name(struct DOU_LINKLIST *phead, char *pname)
{
	struct LIST_NODE *pnode = NULL;
	pnode = phead->pfirst;
	if(NULL == pnode)
	{
		//printf("doubly_linklist is empty\n");
		return NULL;
	}
	while(pnode != NULL)
	{
		if(strcmp(pnode->pdata->name, pname) == 0)
			break;
		pnode = pnode->pnext;
	}
		
	return pnode;
}

struct LIST_NODE* search_list_node_by_passwd(struct DOU_LINKLIST *phead, char *ppasswd)
{
	struct LIST_NODE *pnode = NULL;
	pnode = phead->pfirst;
	if(NULL == pnode)
	{
		//printf("doubly_linklist is empty\n");
		return NULL;
	}
	while(pnode != NULL)
	{
		if(strcmp(pnode->pdata->passwd, ppasswd) == 0)
			break;
		pnode = pnode->pnext;
	}
		
	return pnode;
}


struct LIST_NODE* search_list_node_by_id(struct DOU_LINKLIST *phead, int *pid)
{
	struct LIST_NODE *pnode = NULL;
	pnode = phead->pfirst;
	if(NULL == pnode)
	{
		//printf("doubly_linklist is empty\n");
		return NULL;
	}
	while(pnode != NULL)
	{
		if(pnode->pdata->id == *pid)
			break;
		pnode = pnode->pnext;
	}
		
	return pnode;
}


int modify_list_node(struct LIST_NODE *pnode, struct PERSON_DATA *pdata)
{
	memcpy(pnode->pdata, pdata, sizeof(struct PERSON_DATA));
	return 0;
}



int sort_list_node_by_id(struct DOU_LINKLIST *phead)
{
	int i = 0;
	struct LIST_NODE *pnode =NULL;
	struct LIST_NODE *pnext =NULL;
	struct PERSON_DATA *ptemp = NULL;

	pnode = phead->pfirst;
	if(pnode == NULL)
	{
		//printf("doubly_linklist is empty\n");
		return 1;
	}

	for(i = 0;i<phead->len;i++)
	{
		for(pnode = phead->pfirst;pnode->pnext != NULL;pnode = pnode->pnext)
		{
			pnext = pnode->pnext;
			if( (pnode->pdata->id) > (pnext->pdata->id) )
			{
				ptemp = pnode->pdata;
				pnode->pdata = pnext->pdata;
				pnext->pdata = ptemp;
			}
		}
	}
	return 0;
}


int sort_list_node_by_math_score(struct DOU_LINKLIST *phead)
 {
	int i = 0;
	struct LIST_NODE *pnode =NULL;
	struct LIST_NODE *pnext =NULL;
	struct PERSON_DATA *ptemp = NULL;

	pnode = phead->pfirst;
	if(pnode == NULL)
	{
		//printf("doubly_linklist is empty\n");
		return 1;
	}

	for(i = 0;i<phead->len;i++)
	{
		for(pnode = phead->pfirst;pnode->pnext != NULL;pnode = pnode->pnext)
		{
			pnext = pnode->pnext;
			if( (pnode->pdata->math_score) < (pnext->pdata->math_score) )
			{
				ptemp = pnode->pdata;
				pnode->pdata = pnext->pdata;
				pnext->pdata = ptemp;
			}
		}
	}
	return 0;
 }

int sort_list_node_by_chinese_score(struct DOU_LINKLIST *phead)
{
	int i = 0;
	struct LIST_NODE *pnode =NULL;
	struct LIST_NODE *pnext =NULL;
	struct PERSON_DATA *ptemp = NULL;

	pnode = phead->pfirst;
	if(pnode == NULL)
	{
		//printf("doubly_linklist is empty\n");
		return 1;
	}

	for(i = 0;i<phead->len;i++)
	{
		for(pnode = phead->pfirst;pnode->pnext != NULL;pnode = pnode->pnext)
		{
			pnext = pnode->pnext;
			if( (pnode->pdata->chn_score) < (pnext->pdata->chn_score) )
			{
				ptemp = pnode->pdata;
				pnode->pdata = pnext->pdata;
				pnext->pdata = ptemp;
			}
		}
	}
	return 0;
}

int sort_list_node_by_english_score(struct DOU_LINKLIST *phead)
{
	int i = 0;
	struct LIST_NODE *pnode =NULL;
	struct LIST_NODE *pnext =NULL;
	struct PERSON_DATA *ptemp = NULL;

	pnode = phead->pfirst;
	if(pnode == NULL)
	{
		//printf("doubly_linklist is empty\n");
		return 1;
	}

	for(i = 0;i<phead->len;i++)
	{
		for(pnode = phead->pfirst;pnode->pnext != NULL;pnode = pnode->pnext)
		{
			pnext = pnode->pnext;
			if( (pnode->pdata->eng_score) < (pnext->pdata->eng_score) )
			{
				ptemp = pnode->pdata;
				pnode->pdata = pnext->pdata;
				pnext->pdata = ptemp;
			}
		}
	}
	return 0;
}

int sort_list_node_by_total_score(struct DOU_LINKLIST *phead)
{
	int i = 0;
	float total_now = 0;
	float total_next = 0;
	struct LIST_NODE *pnode =NULL;
	struct LIST_NODE *pnext =NULL;
	struct PERSON_DATA *ptemp = NULL;

	pnode = phead->pfirst;
	if(pnode == NULL)
	{
		//printf("doubly_linklist is empty\n");
		return 1;
	}

	for(i = 0;i<phead->len;i++)
	{
		for(pnode = phead->pfirst;pnode->pnext != NULL;pnode = pnode->pnext)
		{
			pnext = pnode->pnext;
			total_now = pnode->pdata->math_score + pnode->pdata->chn_score + pnode->pdata->eng_score;
			total_next = pnext->pdata->math_score + pnext->pdata->chn_score + pnext->pdata->eng_score;
			if( (total_now) < (total_next) )
			{
				ptemp = pnode->pdata;
				pnode->pdata = pnext->pdata;
				pnext->pdata = ptemp;
			}
		}
	}
	return 0;
}

int print_now_list_node(struct LIST_NODE *pnode)
{
	if(pnode == NULL)
		return 1;

	float total = 0;
	total = pnode->pdata->math_score + pnode->pdata->chn_score + pnode->pdata->eng_score;
	printf("Name:%s\tID:%d\tSex:%c\tMath:%.1f\tChinese:%.1f\tEnglish:%.1f\tTotal-Score:%.1f\t\n",
			pnode->pdata->name, pnode->pdata->id, pnode->pdata->sex, pnode->pdata->math_score, pnode->pdata->chn_score, pnode->pdata->eng_score, total);
	return 0;
}

 int print_doubly_linklist(struct DOU_LINKLIST *phead)
{
	struct LIST_NODE *pnode = phead->pfirst;
	//float total = 0;
	if(phead->pfirst == NULL)
	{
		//printf("doubly_linklist is empty\n");
		return 1;
	}
	while(pnode != NULL)
	{
		//total = pnode->pdata->math_score + pnode->pdata->chn_score + pnode->pdata->eng_score;
		/*
		 printf("Name:%s\tID:%d\tSex:%c\tMath:%.1f\tChinese:%1.f\tEnglish:%1.f\tTotal-Score:%1.f\t\n",
				pnode->pdata->name, pnode->pdata->id, pnode->pdata->sex, pnode->pdata->math_score, pnode->pdata->chn_score, pnode->pdata->eng_score, total);
		*/
		print_now_list_node(pnode);
		pnode = pnode->pnext;
	}
	printf("\n");
	return 0;
}


