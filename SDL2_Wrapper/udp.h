#pragma once
#include <WinSock2.h>
#include <vector>
namespace Net
{
	class UDP
	{
	private:
		SOCKET mRead;
		SOCKET mWrite;
		sockaddr_in mDesti;
		sockaddr_in mSelf;
		int fdmax;
	public:
		UDP(const int port);
		~UDP();
		void broadcast(const std::vector<char> & bytedata);
		std::vector<char> receive();
		void disconnect();
	};
}