#ifndef _COMPUTER_H_
#define _COMPUTER_H_

#include <iostream>
#include <windows.h>
#include <winsock.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Wsock32.lib")

struct NetInfo
{
	char* IP_address;
	char MAC_address16[13];
	char* Name;
	int MAC_address10[6];
};

class computer
{

public:
	NetInfo Info;

	computer();
	computer(NetInfo C);
	computer(char* IP, SOCKET S, WSADATA ws);
	~computer();

	DWORD findMAC();
	void findName();
	int wake_up(SOCKET S, WSADATA ws);
};

#endif // !_COMPUTER_H_