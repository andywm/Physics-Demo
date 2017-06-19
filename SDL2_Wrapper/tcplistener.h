#pragma once
#include <WinSock2.h>
#include <memory>
#include "tcpconnection.h"

namespace Net
{
	class TCPListen
	{
	private:
		SOCKET mSocket;
		unsigned short mPort;
		sockaddr_in mPeer;
		fd_set mFD;
		timeval mTV;
	public:
		TCPListen(const unsigned short port);
		~TCPListen();
		std::shared_ptr<TCPConnection> accept();
		void disconnect();
	};
}