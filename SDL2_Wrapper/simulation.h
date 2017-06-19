/*------------------------------------------------------------------------------
author: andywm, 2016
description:
Primary simulation class, responsible for the main application loop, invokes 
calls to input handling, world updating, and rendering.
------------------------------------------------------------------------------*/
#ifndef SIMULATION_H
#define SIMULATION_H
#include <string>
#include <atomic>
#include "window_archtype.h"
#include "d3d_window.h"
#include "window_manager.h"
#include "environment.h"
#include "Registry.h"
#include "Scene.h"
#include "clock.h"
#include "PhysicsThread.h"
#include "Peer.h"
#include "UI.h"
#include "netphy_communication.h"
class Application
{
	//shared data structures
private:
	std::shared_ptr<Shared::Peer> mNetPeer;
	std::shared_ptr<Shared::Peer> mGfxPeer;
	std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> mGfxBfBalls;
	std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> mGfxBfPeers;
	std::pair<std::atomic<bool>, std::atomic<bool>> mUINetRequestPause;
	std::shared_ptr<Shared::SharingMechanism> mIntersystemExchange;
	std::atomic<bool> mPhysicsPause;
private:
	std::unique_ptr<UI::Statistics> mStats;
	bool mMouseIsCaptured;
	enum class INPUT_SCHEME { DEFAULT };
	struct UI_StateMachine
	{
		INPUT_SCHEME controlMode;
		bool mouseDown;
		bool paused;
	}mUI_State;
	bool mResetRequired;
	bool mTerminate;

	WindowManager mWM;
	D3DWindow mWindow; // WindowArchtype
	std::shared_ptr<Assets::Registry> mRegister;
	std::unique_ptr<Graphics::GraphicsCore> mGraphics;
	std::shared_ptr<Render::Scene::Scene> mScene;

	std::unique_ptr<Physics::PhysicsCore> mPhysics;
	std::unique_ptr<Net::NetworkCore> mNetwork;

	void initialiseSharedObjects();
private:
	int mSystemBallCount;
	//Utilities::Clock mWorldClock;
	void userInputHandling();
	void UI_Default();
	struct Cardinal
	{
		const glm::vec3 FORWARD;
		const glm::vec3	BACKWARD;
		const glm::vec3 LEFT;
		const glm::vec3 RIGHT;
		const glm::vec3 UP;
		const glm::vec3 DOWN;
	}const mCardinals;
	struct Rotational
	{
		const int UP;
		const int DOWN;
		const int LEFT;
		const int RIGHT;
	}const mRotation;

public:
	explicit Application(const std::string & assetDir);
	Application& operator=(const Application&) = delete;
	~Application() = default;
	Application(Application & sim) = delete;
	//const Simulation operator=(const Simulation & sim) = delete;
	//also delete move semantics...

public:	//ui stuff
	bool run();

};
#endif //SIMULATION_H