// dllrecv.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "dllrecv.h"
#include<stdio.h>  
#include<winsock2.h>  
#include<windows.h>  
#include<stdlib.h>
#pragma comment(lib, "ws2_32.lib")

DLLRECV_API int _stdcall my_recv(void)//这个程序
{
#pragma region 公共部分变量定义
	SOCKET sock;
	WSADATA wsdata;
#pragma endregion

#pragma region 接收部分变量定义
	sockaddr_in from, a;
	bool optval;
#pragma endregion

#pragma region 接收操作
	//启动SOCKET库，版本为2.0  
	WSAStartup(0x0202, &wsdata);
	optval = true;
	//然后赋值给两个地址，一个用来绑定套接字，一个用来从网络上的广播地址接收消息；  
	a.sin_family = AF_INET;
	a.sin_addr.s_addr = 0;
	a.sin_port = htons(9000);
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = INADDR_BROADCAST;
	from.sin_port = htons(5050);

	int fromlength = sizeof(SOCKADDR);
	//用UDP初始化套接字  
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	// 设置该套接字为广播类型，  
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char FAR *)&optval, sizeof(optval));
	// 把该套接字绑定在一个具体的地址上  
	if (SOCKET_ERROR == bind(sock, (sockaddr *)&a, sizeof(sockaddr_in))) {
		printf("bind error");
		WSACleanup();
		return 0;
	}
	//获取要接收文件的大小
	SOCKADDR_IN addrS;
	int len = sizeof(sockaddr);
	char strFileSize[20];
	recvfrom(sock, strFileSize, 20, 0, (sockaddr*)&a, &len);
	DWORD dwFileSize;
	dwFileSize = atoi(strFileSize); //文件大小

	DWORD eachRecv = 50 * 1024, n, yu, i, j, retval;
	n = dwFileSize / eachRecv;
	yu = dwFileSize % eachRecv;
	char eachBuf_recv[50 * 1024];


	BYTE *recvbuf = (BYTE*)malloc(dwFileSize);//初始化要存放数据的字符串
	for (i = 0; i<n; i++)
	{
		retval = recvfrom(sock, eachBuf_recv, eachRecv, 0, (sockaddr*)&a, &len);
		if (retval != eachRecv)
		{
			printf("recvfrom error");
			return 0;
		}
		for (j = 0; j<eachRecv; j++)
		{
			recvbuf[i*eachRecv + j] = (BYTE)eachBuf_recv[j];
		}
		memset(eachBuf_recv, 0, 50 * 1024);
	}
	retval = recvfrom(sock, eachBuf_recv, yu, 0, (sockaddr*)&a, &len);
	if (retval != yu)
	{
		printf("recvfrom yu error");
		return 0;
	}
	for (i = 0; i<yu; i++)
	{
		recvbuf[n*eachRecv + i] = (BYTE)eachBuf_recv[i];
	}
	//写入文件
	FILE *fp;
	fp = fopen("E:\\test\\recive.m4a", "wb");
	fwrite(recvbuf, dwFileSize, 1, fp);
	fclose(fp);

	//中断winsocket库
	closesocket(sock);
	WSACleanup();
	return 0;
#pragma endregion

}
// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 dllrecv.h
Cdllrecv::Cdllrecv()
{
    return;
}
