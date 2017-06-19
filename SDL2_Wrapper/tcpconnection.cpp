#include <iterator>
#include <array>
#include "tcpconnection.h"
#include <iostream>
namespace Net
{
	TCPConnection::TCPConnection(SOCKET csock)
		:
		mSocket(csock)
	{
		if (mSocket != INVALID_SOCKET)
		{
			DWORD timeout = 5; int tSize = sizeof(DWORD);
			auto e = WSAGetLastError();
			e = WSAGetLastError();
			setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, tSize);
			setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, tSize);
		}
	}

	TCPConnection::TCPConnection(const std::string & address, unsigned short port)
	{
		char name[256];
		gethostname(name, 256);
		struct in_addr inaddr;
		auto host = gethostbyname(name);
		inaddr.s_addr = ((in_addr*)host->h_addr)->s_addr;
		std::string addr = inet_ntoa(inaddr);

		// Create socket data space 
		mPeer.sin_family = AF_INET;
		mPeer.sin_port = htons(port); // port 9171 
		mPeer.sin_addr.S_un.S_addr = inet_addr(address.c_str());

		// Create socket data space 
		sockaddr_in own;
		own.sin_family = AF_INET;
		own.sin_port = htons(0); // port 9171 
		own.sin_addr.S_un.S_addr = inet_addr(addr.c_str());

		auto e = WSAGetLastError();

		mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (mSocket != INVALID_SOCKET)
		{
			bind(mSocket, (sockaddr*)&own, sizeof(own));
			::connect(mSocket, (sockaddr *)&mPeer, sizeof(mPeer));
			DWORD timeout = 20; int tSize = sizeof(DWORD);
			setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, tSize);
			setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, tSize);
		}
	}

	TCPConnection::TCPConnection(const unsigned int address, unsigned short port)
	{
		char name[256];
		gethostname(name, 256);
		struct in_addr inaddr;
		auto host = gethostbyname(name);
		inaddr.s_addr = ((in_addr*)host->h_addr)->s_addr;
		std::string addr = inet_ntoa(inaddr);

		// Create socket data space 
		mPeer.sin_family = AF_INET;
		mPeer.sin_port = htons(port); // port 9171 
		mPeer.sin_addr.S_un.S_addr = address;

		// Create socket data space 
		sockaddr_in own;
		own.sin_family = AF_INET;
		own.sin_port = htons(0); // port 9171 
		own.sin_addr.S_un.S_addr = inet_addr(addr.c_str());

		auto e = WSAGetLastError();

		mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (mSocket != INVALID_SOCKET)
		{
			bind(mSocket, (sockaddr*)&own, sizeof(own));
			::connect(mSocket, (sockaddr *)&mPeer, sizeof(mPeer));
			DWORD timeout = 20; int tSize = sizeof(DWORD);
			setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, tSize);
			setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, tSize);
		}
	}


	TCPConnection::~TCPConnection()
	{
		if (mSocket != INVALID_SOCKET)
			disconnect();
	}

	void
	TCPConnection::send(const std::vector<char> & msg)
	{
		unsigned int len = msg.size() + 4;
		std::vector<char> msgWithLen(4);
		std::memcpy(msgWithLen.data(), &len, sizeof(unsigned int));

		msgWithLen.insert(msgWithLen.end(), msg.begin(), msg.end());

		auto e = ::send(mSocket,
			msgWithLen.data(),
			msgWithLen.size(),
			0);

		auto v = WSAGetLastError();
		std::cout << v << std::endl;
	}

	std::vector<char>
	TCPConnection::recv()
	{
		bool msgReceived = false;
		int totalRecvd = 0, bytesRecv = 0;
		std::vector<char> fullMessage;
		std::array<char, 256> buffer;

		do
		{
			bytesRecv =
				::recv(mSocket,
					&buffer[0],
					256,
					0);
			totalRecvd += bytesRecv;
			auto e = WSAGetLastError();
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

		} while (totalRecvd < 4 && !msgReceived);

		return fullMessage;
	}

	void
	TCPConnection::disconnect()
	{
		shutdown(mSocket, SD_BOTH);
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}
}