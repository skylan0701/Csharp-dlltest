// dllrecv.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "dllrecv.h"
#include<stdio.h>  
#include<winsock2.h>  
#include<windows.h>  
#include<stdlib.h>
#pragma comment(lib, "ws2_32.lib")

DLLRECV_API int _stdcall my_recv(void)//�������
{
#pragma region �������ֱ�������
	SOCKET sock;
	WSADATA wsdata;
#pragma endregion

#pragma region ���ղ��ֱ�������
	sockaddr_in from, a;
	bool optval;
#pragma endregion

#pragma region ���ղ���
	//����SOCKET�⣬�汾Ϊ2.0  
	WSAStartup(0x0202, &wsdata);
	optval = true;
	//Ȼ��ֵ��������ַ��һ���������׽��֣�һ�������������ϵĹ㲥��ַ������Ϣ��  
	a.sin_family = AF_INET;
	a.sin_addr.s_addr = 0;
	a.sin_port = htons(9000);
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = INADDR_BROADCAST;
	from.sin_port = htons(5050);

	int fromlength = sizeof(SOCKADDR);
	//��UDP��ʼ���׽���  
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	// ���ø��׽���Ϊ�㲥���ͣ�  
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char FAR *)&optval, sizeof(optval));
	// �Ѹ��׽��ְ���һ������ĵ�ַ��  
	if (SOCKET_ERROR == bind(sock, (sockaddr *)&a, sizeof(sockaddr_in))) {
		printf("bind error");
		WSACleanup();
		return 0;
	}
	//��ȡҪ�����ļ��Ĵ�С
	SOCKADDR_IN addrS;
	int len = sizeof(sockaddr);
	char strFileSize[20];
	recvfrom(sock, strFileSize, 20, 0, (sockaddr*)&a, &len);
	DWORD dwFileSize;
	dwFileSize = atoi(strFileSize); //�ļ���С

	DWORD eachRecv = 50 * 1024, n, yu, i, j, retval;
	n = dwFileSize / eachRecv;
	yu = dwFileSize % eachRecv;
	char eachBuf_recv[50 * 1024];


	BYTE *recvbuf = (BYTE*)malloc(dwFileSize);//��ʼ��Ҫ������ݵ��ַ���
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
	//д���ļ�
	FILE *fp;
	fp = fopen("E:\\test\\recive.m4a", "wb");
	fwrite(recvbuf, dwFileSize, 1, fp);
	fclose(fp);

	//�ж�winsocket��
	closesocket(sock);
	WSACleanup();
	return 0;
#pragma endregion

}
// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� dllrecv.h
Cdllrecv::Cdllrecv()
{
    return;
}
