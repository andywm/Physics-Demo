#pragma once

#include <WinSock2.h>
#include <string>
#include <sstream>

unsigned int 
getOwnIPv4Address()
{
	char name[256];
	gethostname(name, 256);
	struct in_addr inaddr;
	auto host = gethostbyname(name);
	inaddr.s_addr = ((in_addr*)host->h_addr)->s_addr;
	std::string addr = inet_ntoa(inaddr);

	std::istringstream iss(addr, std::ios::in);
	std::string result = "";

	unsigned int address = 0;
	int i = 3;
	while (std::getline(iss, result, '.') && i!=-1)
	{
		unsigned int value = std::stoi(result);
		std::memcpy(reinterpret_cast<unsigned char*>(&address) + i,
			&value, 1);
		result = "";
		i--;
	}

	return address;
}
