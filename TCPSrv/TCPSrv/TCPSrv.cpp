// TCPSrv.cpp : �������̨Ӧ�ó������ڵ㡣
//
/*
#include "stdafx.h"
#include <Winsock2.h>//�Ӳ�ͷ�ļ�
#include <stdio.h>//���ر�׼�������ͷ�ļ�

void main()
{
	WORD wVersionRequested;//�汾��
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);//1.1�汾���׽���

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}//�����׽��ֿ⣬�Ӳ�ʧ���򷵻�


	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return;
	}//�������1.1�����˳�
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);//�����׽��֣�socket����

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//ת��Unsigned shortΪ�����ֽ���ĸ�ʽ
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);

	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	//���׽��ְ󶨵�һ�����ص�ַ�Ͷ˿��ϣ�bind��
	listen(sockSrv, 5);//���׽�����Ϊ����ģʽ��׼�����տͻ�����listen����

	SOCKADDR_IN addrClient;//�����ַ��
	int len = sizeof(SOCKADDR);//��ʼ���������������������뱻��ʼ��

	while (1)
	{
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		//accept�ĵ���������һ��Ҫ�г�ʼֵ��
		//�ȴ��ͻ������������������󣬽����������󣬷���һ���µĶ�Ӧ�ڴ˴����ӵ��׽��֣�accept����
		//��ʱ�����ڴ˷�������
		char sendBuf[100];
		sprintf(sendBuf, "Welcome %s to Xuhaoda's Service\nNow Please input your command",
			inet_ntoa(addrClient.sin_addr));
		//�÷��ص��׽��ֺͿͻ��˽���ͨ�ţ�send/recv����
		send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		char recvBuf[100];
		recv(sockConn, recvBuf, 100, 0);
		printf("%s\n", recvBuf);
		if (recvBuf == "�ļ�")
		{
				send(sockConn, recvBuf, strlen(recvBuf) + 1, 0);
		}
		
		closesocket(sockConn);//�ر��׽��֡��ȴ���һ���û�����
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
	// ��������ʼ��һ�������(����)�ĵ�ַ�ṹ 
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// ��ʼ��socket dll 
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("Init socket dll error!");
		exit(1);
	}

	// ����socket 
	SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == m_Socket)
	{
		printf("Create Socket Error!");
		exit(1);
	}

	//��socket�ͷ����(����)��ַ 
	if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("Server Bind Failed: %d", WSAGetLastError());
		exit(1);
	}

	//���� 
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

		FILE * fp = fopen(file_name, "rb"); //windows����"rb",��ʾ��һ��ֻ���Ķ������ļ� 
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
	//�ͷ�winsock�� 
	WSACleanup();
	return 0;
}
