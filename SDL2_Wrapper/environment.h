#pragma once
#include<memory>
#include<map>
#include<string>
#include<vector>
#include "Scene.h"

#include "Registry.h"
#include "so_includes.h"
#include "DoubleBuffer.h"

#include "QuickTransferStruct.hpp"
#include "Peer.h"
#include "PhysicsThread.h"
//#include "so_sphere.h"
#include "network_thread.h"
#include "Weight.hpp"

namespace Graphics
{
	class GraphicsCore
	{
	public:
		explicit GraphicsCore(
			std::shared_ptr<Assets::Registry> reg,
			std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> phyBalls,
			std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> netPeers,
			std::shared_ptr<Shared::Peer> ownPeer,
			const BufferObjs::Weight & weight);
		~GraphicsCore();
		Object::Prop makeObject(const std::string & type, const glm::vec3 & overridePos);
		void generateWorld();
		std::shared_ptr<Render::Scene::Scene> scene() const;
		void updateScene();
		void preallocateProps(int count);

		//shared data structures
	private:
		BufferObjs::Weight mWeight;
		std::shared_ptr<Shared::Peer> mOwnPeer;
		std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> mPhyBalls;
		std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> mNetPeers;
	private:
		std::shared_ptr<Assets::Material> mLight;
		std::shared_ptr<Assets::Material> mMedium;
		std::shared_ptr<Assets::Material> mHeavy;
		std::shared_ptr<Assets::Material> mLightContested;
		std::shared_ptr<Assets::Material> mMediumContested;
		std::shared_ptr<Assets::Material> mHeavyContested;
		std::map<int, std::shared_ptr<Assets::Material>> mMaterialColour;
		//std::vector<std::shared_ptr<Object::Base>> mPeerRegions;	
		//std::vector<std::shared_ptr<Object::Base>> mStaticStuff;
		//std::vector<std::shared_ptr<Object::Base>> mGraphicsBalls;
		std::string mConfigLocation;
		std::shared_ptr<Assets::Registry> mAssetRegistry;
		std::shared_ptr<Render::Scene::Scene> mScene;
	};
}