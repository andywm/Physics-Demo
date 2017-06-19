#include "tcplistener.h"
#include <iostream>
namespace Net
{ 
	TCPListen::TCPListen(const unsigned short port)
		:
		mPort(port)
	{
		// Create socket data space 
		mPeer.sin_family = AF_INET;
		mPeer.sin_port = htons(mPort); // port 9171 
		mPeer.sin_addr.S_un.S_addr = INADDR_ANY;

		mSocket = ::socket(AF_INET, SOCK_STREAM, 0);

		if (mSocket != INVALID_SOCKET)
		{
			if (bind(mSocket, (sockaddr *)&mPeer, sizeof(mPeer)) != SOCKET_ERROR)
			{
				::listen(mSocket, SOMAXCONN);
			}
		}
		u_long iMode = 1;
		//ioctlsocket(mSocket, FIONBIO, &iMode);
	}

	TCPListen::~TCPListen()
	{
		if (mSocket != INVALID_SOCKET)
			disconnect();
	}

	std::shared_ptr<TCPConnection>
	TCPListen::accept()
	{
		FD_ZERO(&mFD);
		FD_SET(mSocket, &mFD);

		mTV.tv_sec = 2;
		mTV.tv_usec = 0;
		
		//select(0, &mFD, NULL, NULL, &mTV);
		
		//if (FD_ISSET(mSocket, &mFD))
		{
			if (WSAGetLastError() == 0)
			{
				SOCKET connSocket = ::accept(mSocket, nullptr, nullptr);
				auto e = WSAGetLastError();
				std::cout << e;
				return std::shared_ptr<TCPConnection>(
					new TCPConnection(connSocket));
			}
		}

		return std::shared_ptr<TCPConnection>(nullptr);
	}

	void
	TCPListen::disconnect()
	{
		shutdown(mSocket, SD_BOTH);
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}
}