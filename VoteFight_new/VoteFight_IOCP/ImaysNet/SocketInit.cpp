#include "../pch.h"
#include "SocketInit.h"

SocketInit g_socketInit;

SocketInit::SocketInit()
{
#ifdef _WIN32
	// Windows에서는 WSAStartup, WSACleanup이 최초와 최후에 딱 한번 있어야 합니다.
	WSADATA w;
	WSAStartup(MAKEWORD(2, 2), &w);
#endif

}

void SocketInit::Touch()
{
}

