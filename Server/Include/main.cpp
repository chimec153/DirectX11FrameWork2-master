#include "Server.h"

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLNT	256
#define BUF_SIZE	1024

HANDLE	g_hMutex;
SOCKET	g_tServ;
SOCKADDR_IN	g_tSock;
SOCKET	g_tClnt[MAX_CLNT];
int		g_iClnt;

unsigned WINAPI Test(void*);

int main(int argc, char* argv[])
{
	if (argc < 1)
	{
		return -1;
	}

	g_hMutex = CreateMutex(NULL, 0, NULL);

	WSAData tData = {};
	WSAStartup(MAKEWORD(2, 2), &tData);

	g_tServ = socket(AF_INET, SOCK_STREAM, 0);

	memset(&g_tSock, 0, sizeof(g_tSock));
	g_tSock.sin_addr.s_addr = htonl(INADDR_ANY);
	g_tSock.sin_port = htons(atoi("9190"));
	g_tSock.sin_family = AF_INET;

	if (bind(g_tServ, (sockaddr*)&g_tSock, sizeof(g_tSock)) == SOCKET_ERROR)
	{
		std::cout << "bind() error" << std::endl;
		return -1;
	}

	if (listen(g_tServ, 0) == SOCKET_ERROR)
	{
		std::cout << "listen() error" << std::endl;
		return -1;
	}

	int iSize = sizeof(g_tSock);
	while (true)
	{
		SOCKADDR_IN tClntSock;
		SOCKET tClnt = accept(g_tServ, (sockaddr*)&tClntSock, &iSize);

		WaitForSingleObject(g_hMutex, INFINITE);
		g_tClnt[g_iClnt++] = tClnt;
		ReleaseMutex(g_hMutex);

		std::cout << "Connect: " << inet_ntoa(tClntSock.sin_addr) << std::endl;

		uintptr_t hThread = _beginthreadex(NULL, 0, Test, (void*)&tClnt, 0, NULL);
	}
	closesocket(g_tServ);
	WSACleanup();

	return 0;
}

unsigned __stdcall Test(void* pData)
{
	SOCKET tSock = *((SOCKET*)pData);
	char strMsg[BUF_SIZE] = {};
	WaitForSingleObject(g_hMutex, INFINITE);

	for (int i = 0; i < g_iClnt; ++i)
	{
		if (g_tClnt[g_iClnt] == tSock)
		{
			while (i++ < g_iClnt - 1)
				g_tClnt[i] = g_tClnt[i + 1];
			break;
		}
	}
	ReleaseMutex(g_hMutex);

	while (recv(tSock, strMsg, BUF_SIZE, 0) != -1)
	{
		WaitForSingleObject(g_hMutex, INFINITE);
		for (int i = 0; i < g_iClnt; ++i)
		{
			send(g_tClnt[i], strMsg, BUF_SIZE, 0);
		}
		ReleaseMutex(g_hMutex);
	}

	return 0;
}
