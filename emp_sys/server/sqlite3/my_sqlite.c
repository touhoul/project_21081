#include "my_sqlite.h"

sqlite3* init_network()
{
	int ret = 0;
	char buf[BUF_LEN] = "";
	sqlite3 *db = NULL;
	struct RESULT presult;

	//打开数据库
	ret = sqlite3_open("./sqlite3/sq.db", &db);
	if(ret != SQLITE_OK)
	{
		printf("数据库打开失败...\n");
		printf("sqlite3_open:(%d)%s\n", sqlite3_errcode(db), sqlite3_errmsg(db));
		return NULL;
	}
	printf("数据库打开成功...\n");
	//创建表格(employees)
		//employees列名： id(主键)、name、age、sex、phone、address、wage、passwd、status
	printf("正在创建表格...\n");
	ret = do_create_table(db);
	if(ret < 0)
	{
		printf("表格创建错误...\n");
		return NULL;
	}
	printf("所有表格创建完成...\n");

	//将管理员信息插入到数据库employees表中,默认初始密码为666666
	sprintf(buf, "%c%c%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c", \
		'R', 48, '#', "Admin", '#', "*", '#', "*", '#', "*", '#', "*", '#', "*", '#', "666666", '#');
	do_insert(db, "employees", buf);
	do_search(db, &presult, "employees", "0");
	printf("当前管理员: id=%s,name=%s,passwd=%s,  %s:%s:__%d__\n", \
		presult.result[9], presult.result[10], presult.result[16], __FILE__,__func__,__LINE__);


	return db;
}

int do_create_table(sqlite3 *db)
{
	int ret = 0;
	//char table[TABLE_LEN] = "";
	char sql[SQL_LEN] = "";
	char *errmsg = NULL;
	
	//employees
	sprintf(sql, "create table if not exists employees (id char primary key, name char, age char, sex char, phone char, address char, wage char, passwd char, status char);");
	ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret != SQLITE_OK)
	{
		printf("employees表格创建失败...\n");
		printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}
	printf("employees表格创建成功...\n");
/*	
	//dict
	bzero(sql, sizeof(sql));
	sprintf(sql, "create table dict (word char, mean char);");
	ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret != SQLITE_OK)
	{
		if(sqlite3_errcode(db) != 1)
		{
			printf("dict表格创建失败...\n");
			printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
			return -1;
		}
	}
	printf("dict表格创建成功...\n");
	//导入词典(dict)
	printf("正在导入词库...\n");
	if(sqlite3_errcode(db) != 1)
	{
		ret = do_import(db);
		if(ret < 0)
		{
			printf("词库导入失败...\n");
			return -1;
		}
	}
	printf("词库导入完成...\n");
	
	//history
	bzero(sql, sizeof(sql));
	sprintf(sql, "create table if not exists history (time char, name char, word char, mean char);");
	ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret != SQLITE_OK)
	{
		printf("history表格创建失败...\n");
		printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}
	printf("history表格创建成功...\n");
*/	
	return 0;
}

int do_insert(sqlite3 *db, char *table, char *pbuf)
{
	int ret = 0;
	char buf[BUF_LEN] = "";
	char sql[SQL_LEN] = "";
	char id[ID_LEN] = "";
	char name[NAME_LEN] = "";
	char age[AGE_LEN] = "";
	char sex[SEX_LEN] = "";
	char addr[ADDR_LEN] = "";
	char phone[PHONE_LEN] = "";
	char wage[WAGE_LEN] = "";
	char passwd[PASSWD_LEN] = "";
	char *errmsg = NULL;
	
	strcpy(buf, pbuf);
	if(0 == strcmp(table, "employees"))
	{
		sscanf(buf+1,"%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]",id,name,age,sex,phone,addr,wage,passwd);
		printf("name=%s,passwd=%s,  %s:%s:__%d__\n", name, passwd, __FILE__,__func__,__LINE__);
		sprintf(sql, "insert into %s values(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\");", \
			table, id, name, age, sex, phone, addr, wage, passwd, "off");
		ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
		if(ret != SQLITE_OK)
		{
			if(1 == sqlite3_errcode(db))
			{
				printf("表格不存在\n");
				printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
				return -1;
			}else if((19 == sqlite3_errcode(db)) && (0 != strcmp(id, "0")))
			{
				printf("id已存在，插入失败\n");
				printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
				return 1;
			}else if(0 != strcmp(id, "0"))
			{
				printf("插入失败\n");
				printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
				return -1;
			}
		}
	}
	printf("插入成功\n\n");
	return 0;
}

int do_delete(sqlite3 *db, char *table, char *pbuf)
{
	int ret = 0;
	char buf[BUF_LEN] = "";
	char sql[SQL_LEN] = "";
	char id[ID_LEN] = "";
	char *errmsg = NULL;

	strcpy(buf, pbuf);
	if(0 == strcmp(table, "employees"))
	{
		sscanf(buf+1,"%[^#]", id);
		sprintf(sql, "delete from %s where id=\"%s\";", table, id);
		ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
		if(ret != SQLITE_OK)
		{
			printf("sqlite3_errcode(db)=%d\n",sqlite3_errcode(db));
			
			if(1 == sqlite3_errcode(db))
			{
				printf("表格不存在\n");
				printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
				return -1;
			}else if(19 == sqlite3_errcode(db))
			{
				printf("id不存在，删除失败\n");
				printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
				return 1;
			}else
			{
				printf("删除失败\n");
				printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
				return -1;
			}
		}
	}
	printf("删除成功\n\n");

	return 0;
}

#if 0
static int callback_select(void *arg, int column, char **column_text, char **column_name)
{
	int i = 0;
	int *flag = (int*)arg;
	//printf("列数=%d\n", column);
	if(*flag)
	{
		for(i=0;i<column;i++)
		{
			printf("%s\t", column_name[i]);
		}
		printf("\n---------------------\n");
		*flag = 0;
	}
	for(i=0;i<column;i++)
	{
		printf("%s\t", column_text[i]);
	}
	printf("\n");

	return 0;
}

int do_search_by_id(sqlite3 *db)
{
	int ret = 0;
	int id = 0;
	int flag = 1;
	char sql[SQL_LEN] = "";
	char table[TABLE_LEN] = "";
	char *errmsg = NULL;

	printf("请输入要查找数据的表格名:");
	scanf("%s", table);
	while(getchar()!='\n');

	printf("请输入要查询的id:");
	scanf("%d", &id);
	while(getchar()!='\n');

	sprintf(sql, "select * from %s where id=%d;", table, id);
	ret = sqlite3_exec(db, sql, callback_select, &flag, &errmsg);
	if(ret != SQLITE_OK)
	{
		if(1 == sqlite3_errcode(db))
		{
			printf("表格不存在\n");
		}else
			printf("查找失败\n");
		printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}

	return 0;
}
#endif

int  do_search(sqlite3 *db, struct RESULT *presult, char *table, char *pid)
{
	int ret = 0;
	//char buf[BUF_LEN] = "";
	char sql[SQL_LEN] = "";
	char id[ID_LEN] = "";
	char *errmsg = NULL;

	//strcpy(buf, pbuf);
	if(0 == strcmp(table, "employees"))
	{
		//sscanf(buf+1, "%[^#]#%s", name, passwd);
		strcpy(id, pid);
		sprintf(sql, "select * from %s where id=\"%s\";", table, id);

		ret = sqlite3_get_table(db, sql, &presult->result, &presult->line, &presult->list, &errmsg);
		if(ret != SQLITE_OK)
		{
			if(1 == sqlite3_errcode(db))
			{
				printf("表格不存在\n");
			}else
				printf("查找失败\n");
			printf("sqlite3_get_table:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
			return -1;
		}
	}

	if(0 == presult->line)
	{
		printf("数据库中未查找到数据  __%d__\n",__LINE__);
		return -1;
	}

	return 0;
}

int get_table_maxID(sqlite3 *db)
{
	int ret = 0;
	int ID_max = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char sql[SQL_LEN] = "";
	char table[TABLE_LEN] = "employees";
	char *errmsg = NULL;
	char **pazResult = NULL;

	sprintf(sql, "select * from %s;", table);
	ret = sqlite3_get_table(db, sql, &pazResult, &pnRow, &pnColumn, &errmsg);
	if(ret != SQLITE_OK)
	{
		if(1 == sqlite3_errcode(db))
		{
			printf("表格不存在\n");
		}else
			printf("查找失败\n");
		printf("sqlite3_get_table:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}

	ID_max = (int)(*pazResult[pnRow * 9]) - 48;//最后一行的(int)id
	sqlite3_free_table(pazResult);

	return ID_max;
}

#if 0
//方法一
/*
int do_display_all(sqlite3 *db)
{
	int ret = 0;
	int flag = 1;
	char sql[SQL_LEN] = "";
	char table[TABLE_LEN] = "";
	char *errmsg = NULL;

	printf("请输入表格名:");
	scanf("%s", table);
	while(getchar()!='\n');

	sprintf(sql, "select * from %s;", table);
	ret = sqlite3_exec(db, sql, callback_select, &flag, &errmsg);
	if(ret != SQLITE_OK)
	{
		if(1 == sqlite3_errcode(db))
		{
			printf("表格不存在\n");
		}else
			printf("查找失败\n");
		printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}

	return 0;
}
*/



//方法二
int do_display_all(sqlite3 *db)
{
	int ret = 0;
	int line = 0;
	int list = 0;
	int n = 0;
	int pnRow = 0;
	int pnColumn = 0;
	char sql[SQL_LEN] = "";
	char table[TABLE_LEN] = "";
	char *errmsg = NULL;
	char **pazResult = NULL;

	printf("请输入要查找数据的表格名:");
	scanf("%s", table);
	while(getchar()!='\n');

	sprintf(sql, "select * from %s;", table);
	ret = sqlite3_get_table(db, sql, &pazResult, &pnRow, &pnColumn, &errmsg);
	if(ret != SQLITE_OK)
	{
		if(1 == sqlite3_errcode(db))
		{
			printf("表格不存在\n");
		}else
			printf("查找失败\n");
		printf("sqlite3_get_table:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}

	for(line=0;line<(pnRow+1);line++)
	{
		for(list=0;list<pnColumn;list++)
		{
			printf("%s\t", pazResult[n]);
			n++;
		}
		printf("\n");
	}

	sqlite3_free_table(pazResult);
	return 0;
}
#endif

#if 0
int do_import(sqlite3 *db)
{
	int ret = 0;
	char buf[100] = "";
	char word[20] = "";
	char mean[50] = "";
	//char *p = NULL;
	//char *q = NULL;
	char sql[SQL_LEN] = "";
	FILE *pfile = NULL;
	char *errmsg = NULL;
	
	pfile = fopen("./sqlite3/dict.txt", "r");
	while(fgets(buf, sizeof(buf), pfile) != NULL)
	{
		bzero(word, sizeof(word));
		bzero(mean, sizeof(mean));
/*
		p = buf;
		q = word;
		while(*p != ' ')
		{
			*q++ = *p++;
			*q = '\0';
		}
		while(*p == ' ')
		{
			p++;
		}
		q = mean;
		while(*p != '\n')
		{
			*q++ = *p++;
			*q = '\0';
		}
*/

		sscanf(buf, "%[^ ]%*[^a-z]%[^\n]", word, mean);
		sprintf(sql, "insert into dict values(\"%s\", \"%s\");", word, mean);
		ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
		if(ret != SQLITE_OK)
		{
			printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
			return -1;
		}
		bzero(buf, sizeof(buf));
	}

	return 0;
}
#endif