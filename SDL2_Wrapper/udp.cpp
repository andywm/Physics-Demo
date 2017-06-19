#include <iterator>
#include <array>
#include <iostream>
#include <string>
#include <cstdint>
#include "udp.h"

namespace Net
{ 
	UDP::UDP(const int port)
	{
		char name[256];
		gethostname(name, 256);
		sockaddr in;
		struct in_addr inaddr;
		auto host = gethostbyname(name);
		inaddr.s_addr = ((in_addr*)host->h_addr)->s_addr;
		std::string addr = inet_ntoa(inaddr);

		//auto addr = getOwnIPAddressStr();
		// Create socket data space 
		mDesti.sin_family = AF_INET;
		mDesti.sin_port = htons(port);
		mDesti.sin_addr.S_un.S_addr = INADDR_BROADCAST;

		mSelf.sin_family = AF_INET;
		mSelf.sin_port = htons(port);
		mSelf.sin_addr.S_un.S_addr = inet_addr(addr.c_str());

		mWrite = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		mRead = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		bind(mRead, (sockaddr*)&mSelf, sizeof(mSelf));
		//bind(mWrite, (sockaddr*)&mDesti, sizeof(mDesti));
		bool optvalTRUE = true; int sizeTRUE = sizeof(bool);
		DWORD optvalTIMEO = 200; int sizeTIMEO = sizeof(DWORD);

		//performs an implicit bind.
		setsockopt(mRead,  SOL_SOCKET, SO_REUSEADDR, (char*)&optvalTRUE,  sizeTRUE);
		setsockopt(mWrite, SOL_SOCKET, SO_REUSEADDR, (char*)&optvalTRUE,  sizeTRUE);
		setsockopt(mWrite, SOL_SOCKET, SO_BROADCAST, (char*)&optvalTRUE,  sizeTRUE);
		setsockopt(mRead,  SOL_SOCKET, SO_BROADCAST, (char*)&optvalTRUE,  sizeTRUE);
		setsockopt(mRead,  SOL_SOCKET, SO_RCVTIMEO,  (char*)&optvalTIMEO, sizeTIMEO);
		setsockopt(mWrite, SOL_SOCKET, SO_SNDTIMEO,  (char*)&optvalTIMEO, sizeTIMEO);


		auto error = WSAGetLastError();
		//if (mSocket == INVALID_SOCKET)
		//{
		//	std::cerr << "Create socket failed" << std::endl;
		//}
	}

	UDP::~UDP()
	{
		if (mWrite != INVALID_SOCKET)
		{
			disconnect();
		}
	}

	void
	UDP::broadcast(const std::vector<char> & msg)
	{
		unsigned int len = msg.size() + 4;
		std::vector<char> msgWithLen(4);
		std::memcpy(msgWithLen.data(), &len, sizeof(unsigned int));

		msgWithLen.insert(msgWithLen.end(), msg.begin(), msg.end());

		auto sent = ::sendto(mWrite,
			(char*)msgWithLen.data(),
			msgWithLen.size(),
			0,
			(SOCKADDR*)&mDesti,
			sizeof(mDesti));

		auto e = WSAGetLastError();
	}

	std::vector<char>
	UDP::receive()
	{
		bool msgReceived = false;
		int totalRecvd = 0, bytesRecv = 0;
		std::vector<char> fullMessage;
		std::array<char, 256> buffer;
		{
			sockaddr_in who;
			int sizeOfWho = sizeof(who);
			do
			{
				auto error = WSAGetLastError();

				bytesRecv = ::recvfrom(mRead,
					&buffer[0],
					256,
					0,
					(sockaddr*)&who,
					&sizeOfWho);
				totalRecvd += bytesRecv;

				auto v = WSAGetLastError();

				error = WSAGetLastError();

				if (bytesRecv > 0)
				{
					std::copy(buffer.begin(),
						buffer.begin() + bytesRecv,
						std::back_inserter(fullMessage));
				}
				if (totalRecvd > 4)
				{
					unsigned int prefix;
					std::memcpy(&prefix, fullMessage.data(), 4);
					if (prefix == totalRecvd) msgReceived = true;
				}
				else if (totalRecvd < 0) break; //WSAError.

			} while ((totalRecvd < 4 && !msgReceived));
		}

		if (fullMessage.size() > 4)
		{
			fullMessage.erase(fullMessage.begin(), fullMessage.begin() + 4);
		}


		//auto e = WSAGetLastError();
		//std::cout << "recv error: " << e << std::endl;
		return fullMessage;
	}

	void
	UDP::disconnect()
	{
		shutdown(mWrite, SD_BOTH);
		shutdown(mRead, SD_BOTH);
		closesocket(mWrite);
		closesocket(mRead);
		mWrite = INVALID_SOCKET;
		mRead = INVALID_SOCKET;
	}
}