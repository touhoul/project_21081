#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doubly_linklist.h"

int display_main_ui();
int InputUserName(char *pname);
int InputPasswd(char *ppasswd);
int InputPasswdAgain(char *ppasswd);
int InputID(int *pnewid);
int InputSex(char *psex);
int InputMathScore(float *pmath);
int InputChineseScore(float *pchn);
int InputEnglishScore(float *peng);

void PrintErrMsg(char *pstr);

//int print_doubly_linklist(struct DOU_LINKLIST *phead);

int PrintAllStudentByID(struct DOU_LINKLIST *phead);
int PrintAllStudentByMathScore(struct DOU_LINKLIST *phead);
int PrintAllStudentByChineseScore(struct DOU_LINKLIST *phead);
int PrintAllStudentByEnglishScore(struct DOU_LINKLIST *phead);
int PrintAllStudentByTotalScore(struct DOU_LINKLIST *phead);




#endif
