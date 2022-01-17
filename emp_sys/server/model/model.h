#ifndef _MODEL_H_
#define _MODEL_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "./../sqlite3/my_sqlite.h"

#define ERR_MSG(msg, ret) {printf("__%d__\n",__LINE__);\
							perror(msg);\
							return ret;}
#define PORT 6666 //服务器PORT
#define IP "192.168.1.17" //服务器IP, ifconfig获取

int init_server();
int do_register(int newfd, char *pbuf, sqlite3 *db);
int login(int newfd, char *pbuf, sqlite3 *db, char *pid);
//int login_admin(int newfd, char *pbuf, sqlite3 *db, char *pid);
int search(int newfd, char *pbuf, sqlite3 *db);
int info_modify(int newfd, char *pbuf, sqlite3 *db);
//int history(int newfd, char *pbuf, sqlite3 *db);
int passwd_modify(int newfd, char *pbuf, sqlite3 *db);
int back(char *pbuf, sqlite3 *db);
int recv_from_client(struct sockaddr_in cin, int newfd, char *buf, sqlite3 *db, char *id);
int send_to_client(int newfd, char *buf);

#endif
