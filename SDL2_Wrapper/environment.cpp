#include <string>
#include <memory>
#include <sstream> 
#include "environment.h"
#include "Registry.h"
#include "uv_sphere_generator.h"
#include "so_includes.h"
#include "a_world.h"

namespace Graphics
{

	GraphicsCore::GraphicsCore(std::shared_ptr<Assets::Registry> reg,
		std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> phyBalls,
		std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> netPeers,
		std::shared_ptr<Shared::Peer> ownPeer,
		const BufferObjs::Weight & weight)
		:
		mWeight(weight),
		mAssetRegistry(reg),
		mScene(new Render::Scene::Scene()),
		mPhyBalls(phyBalls),
		mNetPeers(netPeers),
		mOwnPeer(ownPeer),
		mLight(new Assets::Material(
			glm::vec4(0.0f, 0.5f, 0.5f, 1.0f),
			glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			3.0f)),
		mLightContested(new Assets::Material(
			glm::vec4(0.0f, 0.5f, 0.5f, 1.0f),
			glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			3.0f)),
		mMedium(new Assets::Material(
			glm::vec4(0.05f, 0.5f, 0.1f, 1.0f),
			glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			3.0f)),
		mMediumContested(new Assets::Material(
			glm::vec4(0.05f, 0.5f, 0.1f, 1.0f),
			glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			3.0f)),
		mHeavy(new Assets::Material(
			glm::vec4(0.7f, 0.2f, 0.2f, 1.0f),
			glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			3.0f)),
		mHeavyContested(new Assets::Material(
			glm::vec4(0.7f, 0.2f, 0.2f, 1.0f),
			glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			3.0f))
	{
		mMaterialColour.insert(std::make_pair(int(weight.light), mLight));
		mMaterialColour.insert(std::make_pair(int(weight.medium), mMedium));
		mMaterialColour.insert(std::make_pair(int(weight.heavy), mHeavy));
	}

	GraphicsCore::~GraphicsCore()
	{			
	}

	void 
	GraphicsCore::preallocateProps(int count)
	{
		std::vector<Object::Prop> graphics;
		for (int i = 0; i<count; i++)
		{
			graphics.push_back(
				makeObject("ball", glm::vec3(100000)));		
		}
		mScene->makeBallStructure(graphics);
	}

	/*
		initialises definite world objects with default values, then overrides
		those values if the worldConfig list says so. Then commits them to the
		world.

		loads in all items from the worldAssets list. Adds them to the entities
		list.
	*/
	Object::Prop
	GraphicsCore::makeObject(const std::string & type, 
		const glm::vec3 & overridePos)
	{
		auto worldReg = mAssetRegistry->worldConfig();
		auto spawnList = worldReg.worldAssets();
		for (auto & item : spawnList)
		{
			if (item.makeA == type)
			{
				return Object::Prop(
						*item.pack,
						overridePos,
						item.orientation,
						item.scale,
						type);
			}
		}
	}

	void
	GraphicsCore::generateWorld()
	{		
		auto & worldConfig = mAssetRegistry->worldConfig();
		auto & spawnList = worldConfig.worldAssets();

		mScene->cameraOveride(worldConfig.cameraConfigs()[0]);

		std::vector<Object::Prop> staticStuff;
		std::vector<Object::Prop> peerRegion;
		

		staticStuff.push_back(makeObject("floor", glm::vec3(0)));
		staticStuff.push_back(makeObject("walls", glm::vec3(0)));

		peerRegion.push_back(makeObject("gwell", glm::vec3(0)));
		for (int i = 0; i < 20; i++)
		{
			peerRegion.push_back(makeObject("gwell", glm::vec3(10000)));
		}
		
		mScene->makeScene(staticStuff);
		mScene->makePeers(peerRegion);
	}

	std::shared_ptr<Render::Scene::Scene>
	GraphicsCore::scene() const
	{
		return mScene;
	}

	void
	GraphicsCore::updateScene() 
	{
		mPhyBalls->swap();
		mNetPeers->swap();
		auto balls = *mPhyBalls->read();
		auto netPeers = *mNetPeers->read();
		auto & sdata = mScene->balls();
		auto & sdata_peers = mScene->peers();

		int id = 0;
		for (auto b : balls)
		{
			auto & tball = sdata[id];
			sdata[id].visible(true);
			tball.position(b.position);
			tball.rotation(b.orientation);
			tball.material(mMaterialColour[(int)b.mass]);
			id++;
		}
		for (id; id < sdata.size(); id++)
		{
			sdata[id].visible(false);
		}

		sdata_peers[0].position(mOwnPeer->position);
		id = 1;
		for (auto p : netPeers)
		{
			auto & tpeer = sdata_peers[id];
			sdata[id].visible(true);
			tpeer.position(p.position);
			id++;
		}
		for (id; id < sdata_peers.size(); id++)
		{
			sdata_peers[id].visible(false);
		}
		

		//mPeerRegions[0]->position(mOwnPeer->position);
		//for (int i=1; i<mPeerRegions.size(); i++)
		{
		//	mPeerRegions[1]->position(mOwnPeer->position);
		}		
	}
}