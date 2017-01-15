// TCPClient.cpp : 定义控制台应用程序的入口点。
//
/*
#include "stdafx.h"
#include <Winsock2.h>
#include <stdio.h>

void main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData); //加载套接字库
	if (err != 0) {
		return;
	}


	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0); //创建套接字（socket）。

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)); //向服务器发出连接请求（connect）。

	char recvBuf[100]; //和服务器端进行通信（send / recv）。
	recv(sockClient, recvBuf, 100, 0);
	printf("%s\n", recvBuf);
	char a[100];
	scanf("%s", a);
	send(sockClient, a, strlen(a) + 1, 0);
	recv(sockClient, recvBuf, 100, 0);
	printf("%s\n", recvBuf);

	closesocket(sockClient); //关闭套接字。
	WSACleanup();//必须调用这个函数清除参数

	scanf("%s", a);
}
*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h> 

#define PORT 8087 
#define SERVER_IP "127.0.0.1" 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 
//#pragma comment(lib, "WS2_32") 

int main()
{
	// 初始化socket dll 
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("Init socket dll error!");
		exit(1);
	}

	//创建socket 
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == c_Socket)
	{
		printf("Create Socket Error!");
		system("pause");
		exit(1);
	}

	//指定服务端的地址 
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);

	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("Can Not Connect To Server IP!\n");
		system("pause");
		exit(1);
	}

	//输入文件名 
	char file_name[FILE_NAME_MAX_SIZE + 1];
	memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
	printf("Please Input File Name On Server: ");
	scanf("%s", &file_name);

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));

	//向服务器发送文件名 
	if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		printf("Send File Name Failed\n");
		system("pause");
		exit(1);
	}

	//打开文件，准备写入 
	
	FILE * fp = fopen("E:\\文本\\关于我\\我的科研\\挑战杯\\2.docx", "wb"); //windows下是"wb",表示打开一个只写的二进制文件 
	if (fp==NULL)
	{
		printf("File: %s Can Not Open To Write\n", file_name);
		system("pause");
		exit(1);
	}
	else
	{
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;
		while ((length = recv(c_Socket, buffer, BUFFER_SIZE, 0)) > 0)
		{
			if (fwrite(buffer, sizeof(char), length, fp) < length)
			{
				printf("File: %s Write Failed\n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_SIZE);
		}

		printf("Receive File: %s From Server Successful!\n", file_name);
	}

	fclose(fp);
	closesocket(c_Socket);

	//释放winsock库 
	WSACleanup();

	system("pause");
	return 0;
}
