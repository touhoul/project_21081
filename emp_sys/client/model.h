#ifndef __MODEL_H_
#define __MODEL_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ERR_MSG(msg, ret) {printf("__%d__\n",__LINE__);\
							perror(msg);\
							return ret;}
#define PORT 6666 //服务器PORT
#define IP "192.168.1.17" //服务器IP
#define BUF_LEN 	256
#define ID_LEN 		16
#define NAME_LEN 	20
#define AGE_LEN 	5
#define SEX_LEN 	5
#define PHONE_LEN 	16
#define ADDR_LEN 	30
#define WAGE_LEN 	20
#define PASSWD_LEN 	20
#define STATUS_LEN 	5
//#define WORD_LEN 30
//#define MEAN_LEN 50
//#define TIME_LEN 30


int loop_main(int sfd);
int loop_usr(int sfd, int *status, char *id);
int loop_admin(int sfd, int *status, char *id);
int do_register(int sfd);
int add_employees(int sfd);
int del_employees(int sfd);
int login(int sfd, char *pid);
int search(int sfd, char *pid);
int search_employees(int sfd);
//int history(int sfd, char *pname);
int passwd_modify(int sfd, char *pid);
int info_mod_usr(int sfd, char *id);
int info_mod_admin(int sfd);
int back(int sfd, char *id);
int recv_from_server(int sfd, char *buf);
int send_to_server(int sfd, char *buf);
int err_msg(int err_code);

#endif
