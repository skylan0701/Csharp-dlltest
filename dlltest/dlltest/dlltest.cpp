// dlltest.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "dlltest.h"
#include<stdio.h>  
#include<winsock2.h>  
#include<windows.h>  
#include<stdlib.h>
#include <string.h>
#include <sstream>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "ws2_32.lib")
using namespace std;

// 这是导出函数的一个示例。
void* ReadFile(const char *path, unsigned int *len)
{
	FILE *f = fopen(path, "rb");
	if (f == NULL)
		return NULL;
	fseek(f, 0, SEEK_END);
	*len = ftell(f);
	fseek(f, 0, SEEK_SET);
	void *buffer = malloc(*len);
	*len = fread(buffer, 1, *len, f);
	fclose(f);
	return buffer;
}

//pcm转wav，返回wav内存指针和wav长度  
void* pcmToWav(const void *pcm, unsigned int pcmlen, unsigned int *wavlen)
{
	//44字节wav头  
	void *wav = malloc(pcmlen + 44);
	//wav文件多了44个字节  
	*wavlen = pcmlen + 44;
	//添加wav文件头  
	memcpy(wav, "RIFF", 4);
	*(int *)((char*)wav + 4) = pcmlen + 36;
	memcpy(((char*)wav + 8), "WAVEfmt ", 8);
	*(int *)((char*)wav + 16) = 16;
	*(short *)((char*)wav + 20) = 1;
	*(short *)((char*)wav + 22) = 1;
	*(int *)((char*)wav + 24) = 8000;
	*(int *)((char*)wav + 28) = 16000;
	*(short *)((char*)wav + 32) = 16 / 8;
	*(short *)((char*)wav + 34) = 16;
	strcpy((char*)((char*)wav + 36), "data");
	*(int *)((char*)wav + 40) = pcmlen;

	//拷贝pcm数据到wav中  
	memcpy((char*)wav + 44, pcm, pcmlen);
	return wav;
}

//pcm文件转wav文件，pcmfilePath:pcm文件路劲，wavfilePath:生成的wav路劲  
int pcmfileToWavfile(const char *pcmfilePath, const char *wavfilePath)
{
	unsigned int pcmlen;
	//读取文件获得pcm流，也可以从其他方式获得  
	void *pcm = ReadFile(pcmfilePath, &pcmlen);
	if (pcm == NULL)
	{
		printf("not found file\n");
		return 1;
	}

	//pcm转wav  
	unsigned int wavLen;
	void *wav = pcmToWav(pcm, pcmlen, &wavLen);

	FILE *fwav = fopen(wavfilePath, "wb");
	fwrite(wav, 1, wavLen, fwav);
	fclose(fwav);
	free(pcm);
	free(wav);
	return 0;
}
HWAVEIN hWaveIn;//输入设备
WAVEFORMATEX waveform;//采集音频的格式，结构体
BYTE *pBuffer1;//采集音频时的数据缓存
WAVEHDR wHdr1;//采集音频时包含数据缓存的结构体
FILE *pf;
DLLTEST_API int _stdcall my_send(char *filename)//这个程序
{
#pragma region 公共部分变量定义
	SOCKET sock;
	WSADATA wsdata;
#pragma endregion

#pragma region 发送部分变量定义
	sockaddr_in addrto;
	bool bsocket;
#pragma endregion

#pragma region 发送操作
	//启动SOCKET库，版本为2.0  
	WSAStartup(0x0202, &wsdata);

	sock = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	bsocket = true;
	//然后赋值给地址，用来从网络上的广播地址接收消息；  
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = INADDR_BROADCAST;
	addrto.sin_port = htons(9000);
	bool opt = true;
	//设置该套接字为广播类型，  
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char FAR *)&opt, sizeof(opt));
	int nlen = sizeof(addrto);
	BOOL bSuccess;
	HANDLE hFile;
	DWORD dwFileSize, dwHighSize, dwBytesRead;
	hFile = CreateFile((LPCWSTR)filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("创建文件错误");
		getchar();
		return 0;
	}
	dwFileSize = GetFileSize(hFile, &dwHighSize);
	if (dwHighSize)
	{
		CloseHandle(hFile);
		printf("获取文件大小错误");
		getchar();
		return 0;
	}
	char strFileSize[20];
	_itoa(dwFileSize, strFileSize, 10);
	sendto(sock, strFileSize, 20, 0, (sockaddr*)&addrto, nlen);
	//读文件内容到readfile中
	BYTE *readfile;
	readfile = (BYTE*)malloc(dwFileSize);
	bSuccess = ReadFile(hFile, readfile, dwFileSize, &dwBytesRead, NULL);
	CloseHandle(hFile);
	if (!bSuccess || (dwBytesRead != dwFileSize))
	{
		free(readfile);
		printf("读取文件错误");
		getchar();
		return 0;
	}

	//eachSend表示每次发送字符的大小
	DWORD eachSend = 50 * 1024, retval, n, yu, i, j;
	BYTE eachBuf_send[50 * 1024];
	n = dwFileSize / eachSend;  //共需要几次全额发送
	yu = dwFileSize % eachSend; //最后剩下的字符大小
								//先发送n次全额数据
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < eachSend; j++)
		{
			eachBuf_send[j] = readfile[i*eachSend + j];

		}
		retval = sendto(sock, (char*)eachBuf_send, eachSend, 0, (sockaddr*)&addrto, nlen);
		if (retval != eachSend)
		{
			printf("sendto error");
			getchar();
			return 0;
			memset(eachBuf_send, 0, 50 * 1024);
		}
		//再发送剩下的字符
		for (i = 0; i < yu; i++)
		{
			eachBuf_send[i] = readfile[n*eachSend + i];
		}
		retval = sendto(sock, (char*)eachBuf_send, yu, 0, (sockaddr*)&addrto, nlen);
		if (retval != yu)
		{
			printf("sendto error");
			getchar();
			return 0;
		}
		//中断winsocket库
		closesocket(sock);
		WSACleanup();
		printf("mission success!");
		getchar();
	}
#pragma endregion
}
DLLTEST_API int _stdcall my_recv(long &flag1)//这个程序
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
		return -1;
	}
	int flag = 0;
	char i_tmp[2];
	char tmp1[50];
	const char filename1[] = "recive";
	const char filename2[] = ".m4a";
	while (1) {
		if (flag1 == 0) {
			SOCKADDR_IN addrS;
			int len = sizeof(sockaddr);
			char strFileSize[20];
			recvfrom(sock, strFileSize, 20, 0, (sockaddr*)&a, &len);
			DWORD dwFileSize;
			dwFileSize = atoi(strFileSize); //文件大小

			DWORD eachRecv = 50 * 1024, n, yu, i, j, retval;
			n = dwFileSize / eachRecv;
			yu = dwFileSize % eachRecv;
			char eachBuf[50 * 1024];


			BYTE *recvbuf = (BYTE*)malloc(dwFileSize);//初始化要存放数据的字符串
			for (i = 0; i<n; i++)
			{
				retval = recvfrom(sock, eachBuf, eachRecv, 0, (sockaddr*)&a, &len);
				if (retval != eachRecv)
				{
					printf("recvfrom error");
					return 0;
				}
				for (j = 0; j<eachRecv; j++)
				{
					recvbuf[i*eachRecv + j] = (BYTE)eachBuf[j];
				}
				memset(eachBuf, 0, 50 * 1024);
			}
			retval = recvfrom(sock, eachBuf, yu, 0, (sockaddr*)&a, &len);
			if (retval != yu)
			{
				printf("recvfrom yu error");
				return 0;
			}
			for (i = 0; i<yu; i++)
			{
				recvbuf[n*eachRecv + i] = (BYTE)eachBuf[i];
			}
			//写入文件
			_itoa(flag, i_tmp, 10);
			strcpy(tmp1, filename1);
			strcat(tmp1, i_tmp);
			strcat(tmp1, filename2);
			FILE *fp;
			fp = fopen(tmp1, "wb");
			fwrite(recvbuf, dwFileSize, 1, fp);
			fclose(fp);
			flag++;
			if (flag > 20)
				flag = 0;
		}
	}
	
	//中断winsocket库
	closesocket(sock);
	WSACleanup();
	return 0;
#pragma endregion

}
DLLTEST_API int _stdcall luyin(long &flag2) {
	HANDLE wait;
	waveform.wFormatTag = WAVE_FORMAT_PCM;//声音格式为PCM
	waveform.nSamplesPerSec = 8000;//采样率，16000次/秒
	waveform.wBitsPerSample = 16;//采样比特，16bits/次
	waveform.nChannels = 1;//采样声道数，2声道
	waveform.nAvgBytesPerSec = 16000;//每秒的数据率，就是每秒能采集多少字节的数据
	waveform.nBlockAlign = 2;//一个块的大小，采样bit的字节数乘以声道数
	waveform.cbSize = 0;//一般为0

	wait = CreateEvent(NULL, 0, 0, NULL);
	//使用waveInOpen函数开启音频采集
	waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);

	//建立两个数组（这里可以建立多个数组）用来缓冲音频数据
	DWORD bufsize = 1024 * 100;//每次开辟10k的缓存存储录音数据
							   //	int i = 1;


	int j = 10;
	char tmp[2], tmp1[2], tmp2[2], tmp3[2]; 
	while (1) {
		if (flag2 == 1) {
			strcpy(tmp1, "");
			strcpy(tmp2, "");
			strcpy(tmp3, "");
			_itoa(j, tmp, 10);
			strcpy(tmp1, tmp);
			strcpy(tmp2, tmp);
			strcpy(tmp3, tmp);
			strcat(tmp1, "test.pcm");
			fopen_s(&pf, tmp1, "wb");
			int i = 1;
			while (i--)//录制20左右秒声音，结合音频解码和网络传输可以修改为实时录音播放的机制以实现对讲功能
			{
				pBuffer1 = new BYTE[bufsize];
				wHdr1.lpData = (LPSTR)pBuffer1;
				wHdr1.dwBufferLength = bufsize;
				wHdr1.dwBytesRecorded = 0;
				wHdr1.dwUser = 0;
				wHdr1.dwFlags = 0;
				wHdr1.dwLoops = 1;
				waveInPrepareHeader(hWaveIn, &wHdr1, sizeof(WAVEHDR));//准备一个波形数据块头用于录音
				waveInAddBuffer(hWaveIn, &wHdr1, sizeof(WAVEHDR));//指定波形数据块为录音输入缓存
				waveInStart(hWaveIn);//开始录音
				Sleep(500);//等待声音录制0.5s
				waveInReset(hWaveIn);//停止录音
				fwrite(pBuffer1, 1, wHdr1.dwBytesRecorded, pf);
				delete pBuffer1;
				printf("%ds", i);
			}
			fclose(pf);
			strcat(tmp2, "test.pcm");
			strcat(tmp3, "h.wav");
			int ret = pcmfileToWavfile(tmp2, tmp3);
			my_send(tmp3);
			if (ret != 0)
			{
				printf("error pcm to wav\n");
			}
			else
			{
				printf("succ");
			}
			if (j < 1)
				j = 10;
			else 
				j--;
		}
	}
	


	waveInClose(hWaveIn);





	getchar();
	return 0;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 dlltest.h
Cdlltest::Cdlltest()
{
    return;
}
