#include "./model.h"

int main(int argc, const char *argv[])
{
	int sfd = 0;
	int reuse = 1;
	struct sockaddr_in sin;

	//创建字节流套接字
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0)
		ERR_MSG("socket", -1);

	//允许端口快速重用
	if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
		ERR_MSG("setsockopt", -1);

	//填充服务器的地址信息结构体
	sin.sin_family = AF_INET;//地址族
	sin.sin_port = htons(PORT);//服务器端口号的网络字节序
	sin.sin_addr.s_addr = inet_addr(IP);//服务器IP地址的网络字节序
	
	//连接服务器
	if( connect(sfd, (struct sockaddr*)&sin, sizeof(sin)) < 0 )
		ERR_MSG("connect", -1);
	printf("链接服务器成功\n");

	loop_main(sfd);

	//关闭套接字
	close(sfd);

	return 0;
}
