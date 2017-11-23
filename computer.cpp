#include "computer.h"
#include "Helper_W_LAN.h"

using namespace std;

computer::computer()
{

}

computer::computer(NetInfo C)
{
	this->Info.Name = C.Name;
	this->Info.IP_address = C.IP_address;
	for (int i = 0; i < sizeof(C.MAC_address16) - 1; i++)
	{
		this->Info.MAC_address16[i] = C.MAC_address16[i];
	}
	this->Info.MAC_address16[sizeof(C.MAC_address16) - 1] = '\0';
	for (int i = 0; i < sizeof(C.MAC_address10); i++)
	{
		this->Info.MAC_address10[i] = C.MAC_address10[i];
	}
}

computer::computer(char* IP, SOCKET S, WSADATA ws)
{
	this->Info.IP_address = IP;
	this->findMAC();
	this->findName();
}

computer::~computer()
{

}

DWORD computer::findMAC()
{
	IPAddr IP = inet_addr(Info.IP_address);
	DWORD result;
	BYTE* MAC;
	IPAddr SrcIp = 0;
	ULONG physAddrLen;
	ULONG MacAddr[2];
	memset(&MacAddr, 0xff, sizeof(MacAddr));
	result = SendARP(IP, SrcIp, &MacAddr, &physAddrLen);
	MAC = (BYTE*)&MacAddr;
	for (int i = 0; i < 6; i++)
	{
		this->Info.MAC_address10[i] = (int)MAC[i];
	}
	convert10to16(Info.MAC_address16, MAC);
	return result;
}


inline void computer::findName()
{
	int IP = inet_addr(Info.IP_address);
	LPHOSTENT hn;
	hn = gethostbyaddr((LPSTR)&IP, 4, PF_INET);
	this->Info.Name = hn->h_name;
}

int computer::wake_up(SOCKET s, WSADATA ws)
{
	sockaddr_in s_Client, s_Server;

	ZeroMemory(&s_Client, sizeof(s_Client));
	// ˜˜˜ ˜˜˜˜˜˜ (TCP/IP)
	s_Client.sin_family = AF_INET;
	//˜˜˜˜˜ ˜˜˜˜˜˜˜. ˜.˜. TCP/IP ˜˜˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜ ˜ ˜˜˜˜˜˜˜˜ ˜˜˜˜, ˜˜ ˜˜˜ ˜˜˜˜˜˜˜˜ 
	// ˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜ inet_addr.
	s_Client.sin_addr.s_addr = INADDR_ANY;
	// ˜˜˜˜. ˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜ htons ˜˜˜ ˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜ ˜˜˜˜˜ ˜˜ ˜˜˜˜˜˜˜˜ ˜ //TCP/IP ˜˜˜˜˜˜˜˜˜˜˜˜˜.
	s_Client.sin_port = 0;

	bind(s, (struct sockaddr*)&s_Client, sizeof(s_Client));

	s_Server.sin_family = AF_INET;
	s_Server.sin_addr.s_addr = inet_addr("192.168.40.255");
	s_Server.sin_port = htons(9);

	// ˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜˜:
	if (SOCKET_ERROR == (connect(s, (sockaddr *)&s_Server, sizeof(s_Server))))
		return 1;

	char tosend[102];

	/** first 6 bytes of 255 **/
	for (int i = 0; i < 6; i++)
	{
		tosend[i] = 0xFF;
	}
	/** store mac address **/
	unsigned char mac[6];
	for (int i = 0; i < 6; i++)
	{
		mac[i] = Info.MAC_address10[i];
	}

	/** append it 16 times to packet **/
	for (int i = 1; i <= 16; i++) {
		memcpy(&tosend[i * 6], &mac, 6 * sizeof(unsigned char));
	}

	if (SOCKET_ERROR == sendto(s, tosend, sizeof(char) * 102, 0, (struct sockaddr*)&s_Server, sizeof(s_Server)))
	{
		return 2;
	}

	return 0;
}