#pragma once
#include <WinSock2.h>
#include <string>
#include <vector>
namespace Net
{
	class TCPConnection
	{
	private:
		SOCKET mSocket;
		sockaddr_in mPeer;
	public:
		explicit TCPConnection(SOCKET csock);
		TCPConnection(const std::string & address, unsigned short port);
		TCPConnection(const unsigned int address, unsigned short port);
		~TCPConnection();
		void send(const std::vector<char> & msg);
		std::vector<char> recv();
		void disconnect();
	};
}