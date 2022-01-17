#ifndef _MY_SQLITE_H_
#define _MY_SQLITE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#define TIME_LEN 30
#define NAME_LEN 20
#define PASSWD_LEN 20
#define WORD_LEN 30
#define MEAN_LEN 50
#define TABLE_LEN 20
#define SQL_LEN 128
#define BUF_LEN 128

struct RESULT
{
	char **result;
	int line;
	int list;
};

sqlite3* init_network();
int do_create_table(sqlite3 *db);
int do_insert(sqlite3 *db, char *table, char *pbuf);
int  do_search(sqlite3 *db, struct RESULT *presult, char *table, char *pstr);
int do_display_all(sqlite3 *db);
int do_import(sqlite3 *db);

#endif
