#ifndef _HELPER_W_LAN_H_
#define _HELPER_W_LAN_H_

#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include "computer.h"

using namespace std;

//Функция перевода числа до 255 в 16-ричную СС
inline void convert10to16(char* C, BYTE* A)
{
	for (int i = 0, j = 0; i < 12, j < 6; i += 2, j++)
	{
		switch (A[j] / 16)
		{
		case 0:
			C[i] = '0';
			break;
		case 1:
			C[i] = '1';
			break;
		case 2:
			C[i] = '2';
			break;
		case 3:
			C[i] = '3';
			break;
		case 4:
			C[i] = '4';
			break;
		case 5:
			C[i] = '5';
			break;
		case 6:
			C[i] = '6';
			break;
		case 7:
			C[i] = '7';
			break;
		case 8:
			C[i] = '8';
			break;
		case 9:
			C[i] = '9';
			break;
		case 10:
			C[i] = 'A';
			break;
		case 11:
			C[i] = 'B';
			break;
		case 12:
			C[i] = 'C';
			break;
		case 13:
			C[i] = 'D';
			break;
		case 14:
			C[i] = 'E';
			break;
		case 15:
			C[i] = 'F';
			break;
		}

		switch (A[j] % 16)
		{
		case 0:
			C[i + 1] = '0';
			break;
		case 1:
			C[i + 1] = '1';
			break;
		case 2:
			C[i + 1] = '2';
			break;
		case 3:
			C[i + 1] = '3';
			break;
		case 4:
			C[i + 1] = '4';
			break;
		case 5:
			C[i + 1] = '5';
			break;
		case 6:
			C[i + 1] = '6';
			break;
		case 7:
			C[i + 1] = '7';
			break;
		case 8:
			C[i + 1] = '8';
			break;
		case 9:
			C[i + 1] = '9';
			break;
		case 10:
			C[i + 1] = 'A';
			break;
		case 11:
			C[i + 1] = 'B';
			break;
		case 12:
			C[i + 1] = 'C';
			break;
		case 13:
			C[i + 1] = 'D';
			break;
		case 14:
			C[i + 1] = 'E';
			break;
		case 15:
			C[i + 1] = 'F';
			break;
		}
	}
	C[12] = '\0';
}

inline bool addNewItemInFile(computer C)
{
	char buff[24];
	NetInfo N;
	ofstream fout("networkinfo.txt", ios::ate | ios::app);
	ifstream fin("networkinfo.txt");
	streamsize nameLength = MAX_DOMAIN_NAME_LEN, IPLength = 17, buffLength = 24;
	N.Name = new char[18];
	N.IP_address = new char[17];

	while (1)
	{
		fin.getline(N.Name, nameLength, '\t');
		fin.getline(N.IP_address, IPLength, '\t');
		fin.getline(buff, buffLength, '\t');

		if (strncmp(C.Info.Name, N.Name, 5) == 0)
			return FALSE;
		if (fin.fail())
		{
			fin.close();
			break;
		}
		fin.seekg((int)fin.tellg() + 14);
	}

	fout << C.Info.Name << '\t' << C.Info.IP_address << '\t';

	for (int i = 0; i < 5; i++)
	{
		fout << C.Info.MAC_address10[i] << '-';
	}
	fout << C.Info.MAC_address10[5] << '\t';
	fout << C.Info.MAC_address16 << '\n';
	fout.close();
	return TRUE;
}

inline void showComputerInfo(computer C)
{
	cout << C.Info.Name << '\t' << C.Info.IP_address << '\t';
	for (int i = 0; i < 5; i++)
		cout << C.Info.MAC_address10[i] << '-';
	cout << C.Info.MAC_address10[5] << '\t';
	cout << C.Info.MAC_address16 << endl;
}

inline void readInfoFromFile()
{

	char buff[4];
	NetInfo C;
	ifstream fin("networkinfo.txt");
	streamsize nameLength = 18, IPLength = 17, MAC16Length = 13, MAC10elemLength = 4;
	C.Name = new char[18];
	C.IP_address = new char[17];

	while (1)
	{
		//считывание
		fin.getline(C.Name, nameLength, '\t');
		fin.getline(C.IP_address, IPLength, '\t');
		for (int i = 0; i < 5; i++)
		{
			fin.getline(buff, MAC10elemLength, '-');
			C.MAC_address10[i] = atoi(buff);
		}
		fin.getline(buff, MAC10elemLength, '\t');
		C.MAC_address10[5] = atoi(buff);
		fin.getline(C.MAC_address16, MAC16Length, '\n');

		//вывод
		cout << C.Name << '\t' << C.IP_address << '\t' << C.MAC_address16 << '\t';
		for (int i = 0; i < 5; i++)
			cout << C.MAC_address10[i] << '-';
		cout << C.MAC_address10[5] << endl;
		if (fin.fail())
			break;
	}
	fin.close();
}

//Проверяем включен ли компьютер
inline ULONG PING(char* IP)
{
	HANDLE hIcmpFile;
	IPAddr ipaddr = inet_addr(IP);
	DWORD dwRetVal = 0;
	char SendData[32] = "Data Buffer";
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 0;

	hIcmpFile = IcmpCreateFile();
	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = (VOID*)malloc(ReplySize);

	dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
		NULL, ReplyBuffer, ReplySize, 10000);

	if (dwRetVal != 0)
	{
		PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
		return pEchoReply->Status;
	}
	else
	{
		return 1;
	}
}

inline void getObjectsFromFile(vector<computer> &vecComp)
{
	vector<NetInfo> vecNetInfo;
	computer Comp;
	NetInfo C;
	char buff[4];

	ifstream fin("networkinfo.txt");
	streamsize nameLength = MAX_DOMAIN_NAME_LEN, IPLength = 17, MACLength = 13, MAC10elemLength = 4;

	int i = 0;
	while (1)
	{
		vecNetInfo.push_back(C);
		vecNetInfo[i].IP_address = new char[17];
		vecNetInfo[i].Name = new char[18];

		fin.getline(vecNetInfo[i].Name, nameLength, '\t');
		fin.getline(vecNetInfo[i].IP_address, IPLength, '\t');
		for (int j = 0; j < 5; j++)
		{
			fin.getline(buff, MAC10elemLength, '-');
			vecNetInfo[i].MAC_address10[j] = atoi(buff);
		}
		fin.getline(buff, MAC10elemLength, '\t');
		vecNetInfo[i].MAC_address10[5] = atoi(buff);
		fin.getline(vecNetInfo[i].MAC_address16, MACLength, '\n');

		if (fin.fail())
			break;

		vecComp.push_back(Comp);
		vecComp[i].Info = vecNetInfo[i];

		i++;
	}
	fin.close();
}

inline std::vector<char> makeString(computer C)
{
	//вспомогательные переменные
	char buff[4];
	char Ca;
	vector <char> String1;

	//Записываем имя компьютера
	for (int i = 0; i < strlen(C.Info.Name); i++)
	{
		String1.push_back(Ca);
		String1[i] = C.Info.Name[i];
	}
	String1.push_back('\t');
	String1.push_back('\t');

	//Записываем IP-адресс
	for (int i = String1.size(), j = 0; i < (strlen(C.Info.IP_address) + String1.size()), j < strlen(C.Info.IP_address); i++, j++)
	{
		String1.push_back(Ca);
		String1[i] = C.Info.IP_address[j];
	}
	String1.push_back('\t');

	//Записываем MAC-адресс в 10-чной форме
	for (int i = 0; i < 5; i++)
	{
		_itoa(C.Info.MAC_address10[i], buff, 10);
		for (int k = String1.size(), j = 0; i < (5 + String1.size()), j < strlen(buff); j++)
		{
			String1.push_back(Ca);
			String1[k + j] = buff[j];
		}
		String1.push_back('-');
	}
	_itoa(C.Info.MAC_address10[5], buff, 10);
	for (int i = String1.size(), j = 0; i < (strlen(buff) + String1.size()), j < strlen(buff); i++, j++)
	{
		String1.push_back(Ca);
		String1[i] = buff[j];
	}
	String1.push_back('\t');

	//Записывем MAC-адресс в 16-чной форме
	for (int i = 0; i < 10; i += 2)
	{
		for (int k = String1.size(), j = 0; k < (2 + String1.size()), j < 2; k++, j++)
		{
			String1.push_back(Ca);
			String1[k] = C.Info.MAC_address16[j + i];
		}
		String1.push_back('-');
	}
	for (int i = String1.size(), j = 10; i < 2 + String1.size(), j < 13; i++, j++)
	{
		String1.push_back(Ca);
		String1[i] = C.Info.MAC_address16[j];
	}

	String1.shrink_to_fit();
	return String1;
}

inline bool dop(computer A, computer B)
{
	int res = strcmp(A.Info.Name, B.Info.Name);
	return(res < 0);
}

inline void sortVecComp(vector<computer> &vecComp)
{
	auto itBeg = vecComp.begin();
	auto itEnd = vecComp.end();
	sort(itBeg, itEnd, dop);
}

#endif // !_HELPER_W_LAN_H_
