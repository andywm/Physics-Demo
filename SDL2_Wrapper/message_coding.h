#pragma once
#include <vector>
#include <glm\glm.hpp>
#include "address.h"
namespace Net
{
	enum MsgTypeBit
	{
		Ping = 0,
		DontSend = 1,
		TransferBalls = 2,
		ContestBalls = 3,
		ApplyForce = 4,
		PeerLocation = 5,
		Pause = 6,
		Unpause = 7,
		Timescale = 8,
		Invalid = 256
	};
	unsigned int 
	udpTryGetDisallowTransfers(const std::vector<char> & v)
	{
		unsigned int ip = 0;
		if (v.size() == 8)
		{
			if(v[0]=='a' && v[1]=='z' && v[6] == 'h' && v[7] == 'i')
				std::memcpy(&ip, v.data() + 2 * sizeof(char), 4);
		}
		return ip;
	}

	/*----------------------------------------------------------------------*/
	/*----------------Encoders----------------------------------------------*/
	/*----------------------------------------------------------------------*/

	std::vector<char>
	udpMakeHello(const int owned)
	{
		unsigned int address = getOwnIPv4Address();
		std::vector<char> v{ 'z', 'z',(char)Ping ,0,0,0,0, 0,0,0,0};
		std::memcpy(v.data() + 3 * sizeof(char), &address, 4);
		std::memcpy(v.data() + 7 * sizeof(char), &owned, 4);
		return v;
	}

	std::vector<char>
	udpMakeDisallow()
	{
		unsigned int address = getOwnIPv4Address();
		std::vector<char> v{ 'z', 'z',(char)DontSend ,0,0,0,0};
		std::memcpy(v.data() + 3 * sizeof(char), &address, 4);
		return v;
	}
	
	std::vector<char>
	udpMakeTransferBalls(const std::vector<std::vector<char>> & balls,
		const unsigned int target)
	{
		unsigned int address = getOwnIPv4Address();
		unsigned int numBalls = balls.size();
		std::vector<char> v
		{
			'z', 'z',
			(char)TransferBalls,
			0,0,0,0, 
			0,0,0,0, 
			0,0,0,0
		};
		std::memcpy(v.data() + 3 * sizeof(char), &address, 4);

		std::memcpy(v.data() + 7 * sizeof(char), &target, 4);
		std::memcpy(v.data() + 11 * sizeof(char), &numBalls, 4);
		for (auto ball : balls)
		{
			v.insert(v.end(), ball.begin(), ball.end());
		}
		return v;
	}

	std::vector<char>
	udpMakeContestedBalls(const std::vector<std::vector<char>> & balls,
			const unsigned int target)
	{
		unsigned int address = getOwnIPv4Address();
		unsigned int numBalls = balls.size();
		std::vector<char> v
		{
			'z', 'z',
			(char)ContestBalls,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0
		};
		std::memcpy(v.data() + 3 * sizeof(char), &address, 4);

		std::memcpy(v.data() + 7 * sizeof(char), &target, 4);
		std::memcpy(v.data() + 11 * sizeof(char), &numBalls, 4);
		for (auto ball : balls)
		{
			v.insert(v.end(), ball.begin(), ball.end());
		}
		return v;
	}

	std::vector<char>
	udpMakeApplyForce(const std::vector<std::vector<char>> & forces,
			const unsigned int target)
	{
		unsigned int address = getOwnIPv4Address();
		unsigned int numForces = forces.size();
		std::vector<char> v
		{
			'z', 'z',
			(char)ApplyForce,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0
		};
		std::memcpy(v.data() + 3 * sizeof(char), &address, 4);

		std::memcpy(v.data() + 7 * sizeof(char), &target, 4);
		std::memcpy(v.data() + 11 * sizeof(char), &numForces, 4);
		for (auto forces : forces)
		{
			v.insert(v.end(), forces.begin(), forces.end());
		}
		return v;
	}
	std::vector<char>
	udpMakePeerLocation(const glm::vec3 & peer)
	{
		unsigned int address = getOwnIPv4Address();

		std::vector<char> v
		{
			'z', 'z',
			(char)PeerLocation,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0
		};
		std::memcpy(v.data() + 3 * sizeof(char), &address, 4);

		std::memcpy(v.data() + 7 * sizeof(char), &peer.x, 4);
		std::memcpy(v.data() + 11 * sizeof(char), &peer.y, 4);
		std::memcpy(v.data() + 15 * sizeof(char), &peer.z, 4);

		return v;
	}

	std::vector<char>
	udpMakePause()
	{
		unsigned int address = getOwnIPv4Address();

		std::vector<char> v
		{
			'z', 'z',
			(char)Pause,
			0,0,0,0,
			'p'
		};
		std::memcpy(v.data() + 3 * sizeof(char), &address, 4);
		return v;
	}

	std::vector<char>
	udpMakeUnpause()
	{
		unsigned int address = getOwnIPv4Address();

		std::vector<char> v
		{
			'z', 'z',
			(char)Unpause,
			0,0,0,0,
			'u'
		};
		std::memcpy(v.data() + 3 * sizeof(char), &address, 4);
		return v;
	}

	std::vector<char>
	udpMakeTimescale(const float ts)
	{
		unsigned int address = getOwnIPv4Address();

		std::vector<char> v
		{
			'z', 'z',
			(char)Timescale,
			0,0,0,0,
			0,0,0,0
		};
		std::memcpy(v.data() + 3 * sizeof(char), &address, 4);
		std::memcpy(v.data() + 7 * sizeof(char), &ts, 4);
		return v;
	}

	std::vector<char>
	encodeBall(const unsigned int id,
		const glm::vec3 & pos,
		const glm::vec3 & vel,
		const glm::vec3 & ori,
		const glm::vec3 & avel)
	{
		std::vector<char> v
		{
			0,0,0,0,//id
			0,0,0,0,//px
			0,0,0,0,//py
			0,0,0,0,//pz
			0,0,0,0,//vx
			0,0,0,0,//vy
			0,0,0,0,//vz
			0,0,0,0,//ox
			0,0,0,0,//oy
			0,0,0,0,//oz
			0,0,0,0,//ax
			0,0,0,0,//ay
			0,0,0,0,//az
		};
		std::memcpy(v.data(), &id, 4);

		std::memcpy(v.data() + 4 * sizeof(char),  &pos.x, 4);
		std::memcpy(v.data() + 8 * sizeof(char), &pos.y, 4);
		std::memcpy(v.data() + 12 * sizeof(char), &pos.z, 4);

		std::memcpy(v.data() + 16 * sizeof(char),  &vel.x, 4);
		std::memcpy(v.data() + 20 * sizeof(char),  &vel.y, 4);
		std::memcpy(v.data() + 24 * sizeof(char),  &vel.z, 4);

		std::memcpy(v.data() + 28 * sizeof(char),  &ori.x, 4);
		std::memcpy(v.data() + 32 * sizeof(char),  &ori.y, 4);
		std::memcpy(v.data() + 36 * sizeof(char),  &ori.z, 4);

		std::memcpy(v.data() + 40 * sizeof(char),  &avel.x, 4);
		std::memcpy(v.data() + 44 * sizeof(char),  &avel.y, 4);
		std::memcpy(v.data() + 48 * sizeof(char),  &avel.z, 4);
		return v;
	}
	std::vector<char>
	encodeForce(const unsigned int id,
			const glm::vec3 & force)
	{
		std::vector<char> v
		{
			0,0,0,0,//id
			0,0,0,0,//px
			0,0,0,0,//py
			0,0,0,0,//pz
		};
		std::memcpy(v.data(),  &id, 4);

		std::memcpy(v.data() + 4 * sizeof(char), &force.x, 4);
		std::memcpy(v.data() + 8 * sizeof(char), &force.y, 4);
		std::memcpy(v.data() + 12 * sizeof(char), &force.z, 4);
		return v;
	}

	/*----------------------------------------------------------------------*/
	/*----------------Decoders----------------------------------------------*/
	/*----------------------------------------------------------------------*/

	unsigned int
	udpWhoSent(const std::vector<char> & message)
	{
		unsigned int address;
		std::memcpy(&address, message.data() + 3 * sizeof(char), 4);
		return address;
	}

	unsigned int 
	udpWhatMessage(const std::vector<char> & message)
	{
		if (message.size() < 7)
			return Invalid;

		//don't bother with own messages.
		unsigned int address;
		std::memcpy(&address, message.data() + 3 * sizeof(char), 4);
		if (address == getOwnIPv4Address()) return Invalid;

		switch (message[2])
		{
			case (char)Ping:
			{
				return Ping;
			}
			case (char)DontSend:
			{
				return DontSend;
			}
			case (char)TransferBalls:
			{
				return TransferBalls;
			}
			case (char)ContestBalls:
			{
				return ContestBalls;
			}
			case (char)ApplyForce:
			{
				return ApplyForce;
			}
			case (char)PeerLocation:
			{
				return PeerLocation;
			}
			case (char)Pause:
			{
				return Pause;
			}
			case (char)Unpause:
			{
				return Unpause;
			}
			case (char)Timescale:
			{
				return Timescale;
			}
		}
		return Invalid;
	}

	void
	udpDecodeHello(int & owned, std::vector<char> message)
	{
		std::memcpy(&owned, message.data() + 7 * sizeof(char), 4);
	}


	//decode transferballs
	void
	udpDecodeTransferBalls(std::vector<std::vector<char>> & balls,
			unsigned int & target,
			const std::vector<char> & message)
	{
		//std::memcpy(v.data() + 3 * sizeof(char), &address, 4);
		unsigned int numBalls;
		std::memcpy(&target, message.data() + 7 * sizeof(char), 4);
		std::memcpy(&numBalls, message.data() + 11 * sizeof(char), 4);
		for (int i = 0; i < numBalls; i++)
		{
			std::vector<char> newvec;
			newvec.insert(newvec.begin(), message.begin() + 15 + i * 52,
				message.begin() + 15 + (i + 1) * 52);
			balls.push_back(newvec);
		}
	}

	//decode transfercontend
	void
	udpDecodeTransferContended(std::vector<std::vector<char>> & balls,
			unsigned int & target,
			const std::vector<char> & message)
	{
		//std::memcpy(v.data() + 3 * sizeof(char), &address, 4);
		unsigned int numBalls;
		std::memcpy(&target, message.data() + 7 * sizeof(char), 4);
		std::memcpy(&numBalls, message.data() + 11 * sizeof(char), 4);
		for (int i = 0; i < numBalls; i++)
		{
			std::vector<char> newvec;
			newvec.insert(newvec.begin(), message.begin() + 15 + i * 52,
				message.begin() + 15 + (i + 1) * 52);
			balls.push_back(newvec);
		}
	}
	//decode transferforces
	void
	udpDecodeTransferForces(std::vector<std::vector<char>> & forces,
			unsigned int & target,
			const std::vector<char> & message)
	{
		//std::memcpy(v.data() + 3 * sizeof(char), &address, 4);
		unsigned int numForces;
		std::memcpy(&target, message.data() + 7 * sizeof(char), 4);
		std::memcpy(&numForces, message.data() + 11 * sizeof(char), 4);
		for (int i = 0; i < numForces; i++)
		{
			std::vector<char> newvec;
			newvec.insert(newvec.begin(), message.begin() + 15 + i * 16,
				message.begin() + 15 + (i + 1) * 16);
			forces.push_back(newvec);
		}
	}
	//decode transfer peer location
	void
	udpDecodePeerLocation(unsigned int & who,
			glm::vec3 & where,
			const std::vector<char> & message)
	{
		std::memcpy(&who, message.data() + 3 * sizeof(char), 4);
	
		std::memcpy(&where.x, message.data() + 7 * sizeof(char), 4);
		std::memcpy(&where.y, message.data() + 11 * sizeof(char), 4);
		std::memcpy(&where.z, message.data() + 15 * sizeof(char), 4);
	}
	//decode timescale
	void
	udpDecodeTimescale(float & ts,
			const std::vector<char> & message)
	{
		std::memcpy(&ts, message.data() + 7 * sizeof(char), 4);
	}
	//decode ball
	void
	decodeBall(unsigned int & id,
			glm::vec3 & pos,
			glm::vec3 & vel,
			glm::vec3 & ori,
			glm::vec3 & avel,
		const std::vector<char> & vec)
	{
		std::memcpy(&id, vec.data(), 4);

		std::memcpy(&pos.x, vec.data() + 4 * sizeof(char), 4);
		std::memcpy(&pos.y, vec.data() + 8 * sizeof(char), 4);
		std::memcpy(&pos.z, vec.data() + 12 * sizeof(char), 4);

		std::memcpy(&vel.x, vec.data() + 16 * sizeof(char), 4);
		std::memcpy(&vel.y, vec.data() + 20 * sizeof(char), 4);
		std::memcpy(&vel.z, vec.data() + 24 * sizeof(char), 4);

		std::memcpy(&ori.x, vec.data() + 28 * sizeof(char), 4);
		std::memcpy(&ori.y, vec.data() + 32 * sizeof(char), 4);
		std::memcpy(&ori.z, vec.data() + 36 * sizeof(char), 4);

		std::memcpy(&avel.x, vec.data() + 40 * sizeof(char), 4);
		std::memcpy(&avel.y, vec.data() + 44 * sizeof(char), 4);
		std::memcpy(&avel.z, vec.data() + 48 * sizeof(char), 4);
	}
	//decode force
	void
	decodeForce(unsigned int & id,
			glm::vec3 & force,
			const std::vector<char> & vec)
	{

		std::memcpy(&id, vec.data(), 4);

		std::memcpy(&force.x, vec.data() + 4 * sizeof(char), 4);
		std::memcpy(&force.y, vec.data() + 8 * sizeof(char), 4);
		std::memcpy(&force.z, vec.data() + 12 * sizeof(char), 4);
	}
}