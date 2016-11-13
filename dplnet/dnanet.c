#include <Windows.h>
#include <stdio.h>
#include "dnanet.h"

#pragma comment(lib, "ws2_32.lib")

#define WM_SOCKET WM_USER + 1
#define BUFSIZE 512

void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |	FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, 0);
	printf("[%s]%s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
int dnaOpen(char* ip, int port, int type)
{
	int e = -1;
	WSADATA wsa;
	SOCKET sd = 0;
	SOCKADDR_IN addr;
	int on = 1;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}

	if (type & 0x80 == 0x80)  /// UDP
	{
		sd = socket(PF_INET, SOCK_DGRAM, 0);
	}
	else /// TCP
	{
		sd = socket(PF_INET, SOCK_STREAM, 0);
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		e = ioctlsocket(sd, FIONBIO, (unsigned long *)&on);

		if (type & 0x08 == 0x08) /// CLIENT
		{
			addr.sin_addr.s_addr = inet_addr(ip);
			e = connect(sd, (struct sockaddr*)&addr, sizeof(struct sockaddr));
			if (e < 0) sd = e;
		}
		else  /// SERVER
		{
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
			e = bind(sd, (SOCKADDR*)&addr, sizeof(addr));
			e = listen(sd, SOMAXCONN);
		}
	}
	return (int)sd;
}

int dnaClose(int sd)
{
	int e = -1;

	closesocket(sd);

	return e;
}

int dnaAccept(int sd, char* ip, int sz)
{
	int e = -1;
	SOCKADDR_IN clientaddr;
	int addrlen, retval;
	char buf[BUFSIZE + 1];

	addrlen = sizeof(clientaddr);
	e = accept(sd, (SOCKADDR*)&clientaddr, &addrlen);

	//if (e == INVALID_SOCKET)
	//{
	//	err_display("accept()");
	//}
	//printf("\n[TCP ?œë²„] ?´ë¼?´ì–¸???‘ì† : IPì£¼ì†Œ=%s, ?¬íŠ¸ë²ˆí˜¸=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	return e;
}


int dnaRead(int sd, char* buf, int sz, char* ip, int port)
{
	int e = -1;
	struct sockaddr_in addr;
	int _sz = 0;
	if (ip == 0)
	{
		e = recv(sd, buf, sz, 0);
	}
	else
	{
		e = recvfrom(sd, buf, sz, 0, (struct sockaddr*)&addr, &_sz);
	}
	return e;
}

int dnaWrite(int sd, char* buf, int sz, char* ip, int port)
{
	int e = -1;

	if (ip == 0)
	{
		e = send(sd, buf, sz, 0);
	}
	else
	{
		e = sendto(sd, buf, sz, 0, 0, sizeof(struct sockaddr));
	}

	return e;
}
