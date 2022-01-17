#include "my_sqlite.h"

sqlite3* init_network()
{
	int ret = 0;
	sqlite3 *db = NULL;
	//打开数据库
	ret = sqlite3_open("./sqlite3/sq.db", &db);
	if(ret != SQLITE_OK)
	{
		printf("数据库打开失败...\n");
		printf("sqlite3_open:(%d)%s\n", sqlite3_errcode(db), sqlite3_errmsg(db));
		return NULL;
	}
	printf("数据库打开成功...\n");
	//创建表格(usr/dict/history)
		//usr列名： name(主键)、passwd、status
		//dict列名：word、mean
			//导入词典(dict)
		//history列名：time(主键)、name、word、mean
	printf("正在创建表格...\n");
	ret = do_create_table(db);
	if(ret < 0)
	{
		printf("表格创建错误...\n");
		return NULL;
	}
	printf("所有表格创建完成...\n");

	return db;
}


int do_create_table(sqlite3 *db)
{
	int ret = 0;
	//char table[TABLE_LEN] = "";
	char sql[SQL_LEN] = "";
	char *errmsg = NULL;
	
	//usr
	sprintf(sql, "create table if not exists usr (name char primary key, passwd char, status char);");
	ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret != SQLITE_OK)
	{
		printf("usr表格创建失败...\n");
		printf("sqlite3_exec:(%d)%s  __%d__\n", sqlite3_errcode(db), errmsg, __LINE__);
		return -1;
	}
	printf("usr表格创建成功...\n");
	
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
	return 0;
}

int do_insert(sqlite3 *db, char *table, char *pbuf)
{
	int ret = 0;
	char buf[BUF_LEN] = "";
	char sql[SQL_LEN] = "";
	char name[NAME_LEN] = "";
	char passwd[PASSWD_LEN] = "";
	char loc_time[TIME_LEN] = "";
	char word[WORD_LEN] = "";
	char mean[MEAN_LEN] = "";
	char *errmsg = NULL;
	
	strcpy(buf, pbuf);
	if(0 == strcmp(table, "usr"))
	{
		sscanf(buf+1,"%[^#]#%s", name, passwd);
		//printf("name=%s,passwd=%s,  __%d__\n", name, passwd, __LINE__);
		sprintf(sql, "insert into %s values(\"%s\", \"%s\", \"%s\");", table, name, passwd, "off");
		ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
		if(ret != SQLITE_OK)
		{
			if(1 == sqlite3_errcode(db))
			{
				printf("表格不存在\n");
				printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
				return -1;
			}else if(19 == sqlite3_errcode(db))
			{
				printf("name已存在，插入失败\n");
				printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
				return 1;
			}else
			{
				printf("插入失败\n");
				printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
				return -1;
			}
		}
	}
	else if(0 == strcmp(table, "history"))
	{
		sscanf(buf,"%[^#]#%[^#]#%[^#]#%[^#]", loc_time, name, word, mean);
		sprintf(sql, "insert into %s values(\"%s\", \"%s\", \"%s\", \"%s\");", table, loc_time, name, word, mean);
				
		printf("*****  __%s__   __%d__  *****\n", __FILE__, __LINE__);
		
		ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
		if(ret != SQLITE_OK)
		{
			if(1 == sqlite3_errcode(db))
			{
				printf("表格不存在\n");
			}else
				printf("插入失败\n");
			printf("sqlite3_exec:%s  __%d__\n", errmsg, __LINE__);
			return -1;
		}
	}

	printf("插入成功\n\n");
	return 0;
}


/*
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
*/


int  do_search(sqlite3 *db, struct RESULT *presult, char *table, char *pstr)
{
	int ret = 0;
	//char buf[BUF_LEN] = "";
	char sql[SQL_LEN] = "";
	char name[NAME_LEN] = "";
	char word[WORD_LEN] = "";
	char *errmsg = NULL;

	//strcpy(buf, pbuf);
	if(0 == strcmp(table, "usr"))
	{
		//sscanf(buf+1, "%[^#]#%s", name, passwd);
		strcpy(name, pstr);
		sprintf(sql, "select * from %s where name=\"%s\";", table, name);

#if 0
		int sqlite3_get_table(
				sqlite3 *db, // 一个打开的数据库 
				const char *zSql, // 要计算的 SQL 
				char ***pazResult, // 查询结果 
				int *pnRow, // 这里写入的结果行数 
				int *pnColumn, // 此处写入的结果列数 
				char **pzErrmsg // 此处写入错误信息 
				);
		void sqlite3_free_table(char **result);
#endif

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
	}else if(0 == strcmp(table, "dict"))
	{
		strcpy(word, pstr);
		sprintf(sql, "select * from %s where word=\"%s\";", table, word);
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
	}else if(0 == strcmp(table, "history"))
	{
		strcpy(name, pstr);
		sprintf(sql, "select * from %s where name=\"%s\";", table, name);
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

/*
	for(line=0;line<(pnRow+1);line++)
	{
		for(list=0;list<pnColumn;list++)
		{
			printf("%s\t", pazResult[n]);
			n++;
		}
		printf("\n");
	}
*/

	//sqlite3_free_table(pazResult);
	return 0;
}




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
