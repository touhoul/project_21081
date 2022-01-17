#include <stdio.h>

union  dt {
    int a;
    char b;
    double c;
} data;

int main(int argc, const char *argv[])
{
	data.a=5;
	printf("a=%d\n",data.a);
	printf("c=%f\n",data.c);
	return 0;
}
