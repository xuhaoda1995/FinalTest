// TCPSrv.cpp : 定义控制台应用程序的入口点。
//
/*
#include "stdafx.h"
#include <Winsock2.h>//加裁头文件
#include <stdio.h>//加载标准输入输出头文件

void main()
{
	WORD wVersionRequested;//版本号
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);//1.1版本的套接字

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}//加载套接字库，加裁失败则返回


	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return;
	}//如果不是1.1的则退出
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);//创建套接字（socket）。

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//转换Unsigned short为网络字节序的格式
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);

	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	//将套接字绑定到一个本地地址和端口上（bind）
	listen(sockSrv, 5);//将套接字设为监听模式，准备接收客户请求（listen）。

	SOCKADDR_IN addrClient;//定义地址族
	int len = sizeof(SOCKADDR);//初始化这个参数，这个参数必须被初始化

	while (1)
	{
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		//accept的第三个参数一定要有初始值。
		//等待客户请求到来；当请求到来后，接受连接请求，返回一个新的对应于此次连接的套接字（accept）。
		//此时程序在此发生阻塞
		char sendBuf[100];
		sprintf(sendBuf, "Welcome %s to Xuhaoda's Service\nNow Please input your command",
			inet_ntoa(addrClient.sin_addr));
		//用返回的套接字和客户端进行通信（send/recv）。
		send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		char recvBuf[100];
		recv(sockConn, recvBuf, 100, 0);
		printf("%s\n", recvBuf);
		if (recvBuf == "文件")
		{
				send(sockConn, recvBuf, strlen(recvBuf) + 1, 0);
		}
		
		closesocket(sockConn);//关闭套接字。等待另一个用户请求
	}
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
	// 声明并初始化一个服务端(本地)的地址结构 
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// 初始化socket dll 
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("Init socket dll error!");
		exit(1);
	}

	// 创建socket 
	SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == m_Socket)
	{
		printf("Create Socket Error!");
		exit(1);
	}

	//绑定socket和服务端(本地)地址 
	if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("Server Bind Failed: %d", WSAGetLastError());
		exit(1);
	}

	//监听 
	if (SOCKET_ERROR == listen(m_Socket, 10))
	{
		printf("Server Listen Failed: %d", WSAGetLastError());
		exit(1);
	}


	while (1)
	{
		printf("Listening To Client...\n");

		sockaddr_in client_addr;
		int client_addr_len = sizeof(client_addr);

		SOCKET m_New_Socket = accept(m_Socket, (sockaddr *)&client_addr, &client_addr_len);
		if (SOCKET_ERROR == m_New_Socket)
		{
			printf("Server Accept Failed: %d", WSAGetLastError());
			break;
		}

		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("Server Receive Data Failed!");
			break;
		}

		char file_name[FILE_NAME_MAX_SIZE + 1];
		memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
		strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
		printf("%s\n", file_name);

		FILE * fp = fopen(file_name, "rb"); //windows下是"rb",表示打开一个只读的二进制文件 
		if (NULL == fp)
		{
			printf("File: %s Not Found\n", file_name);
		}
		else
		{
			memset(buffer, 0, BUFFER_SIZE);
			int length = 0;

			while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
			{
				if (send(m_New_Socket, buffer, length, 0) < 0)
				{
					printf("Send File: %s Failed\n", file_name);
					break;
				}
				memset(buffer, 0, BUFFER_SIZE);
			}

			fclose(fp);
			printf("File: %s Transfer Successful!\n", file_name);
		}
		closesocket(m_New_Socket);
	}

	closesocket(m_Socket);
	//释放winsock库 
	WSACleanup();
	return 0;
}
