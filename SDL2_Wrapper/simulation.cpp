#ifndef  _WINSOCKAPI_
#define _WINSOCKAPI_
#endif // ! #define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <SDL2\SDL_events.h>
#include <atomic>
#include <glm\glm.hpp>
#include "d3d_gpu_geometry.h"
#include "d3d_context.h"
#include <glm/gtx/transform.hpp>
#include <AntTweakBar.h> 
#include "so_camera.h"
#include "simulation.h"
#include "Scene.h"
#include "master.h"
#include "weight.hpp"


Application::Application(const std::string & assetDir)
	:
	mCardinals 
	{
		/*back   */ glm::vec3(+0.0f, +0.0f, -1.0f),
		/*forward*/ glm::vec3(+0.0f, +0.0f, +1.0f),
		/*left   */ glm::vec3(+1.0f, +0.0f, +0.0f),
		/*right  */ glm::vec3(-1.0f, +0.0f, +0.0f),
		/*up     */ glm::vec3(+0.0f, -1.0f, +0.0f),
		/*down   */ glm::vec3(+0.0f, +1.0f, +0.0f)
	},
	mRotation
	{
		/*up     */ +1,
		/*down   */ -1,
		/*left   */ +1,
		/*right  */ -1
	},
	mTerminate(false),
	mResetRequired(false),
	mWindow("Gravity Wells ACW (08024)", 1000, 800, 0),
	mUI_State{ INPUT_SCHEME::DEFAULT, false,  false},
	mMouseIsCaptured(true),
	mRegister(std::shared_ptr<Assets::Registry>(new Assets::Registry(assetDir)))
{
	initialiseSharedObjects();
	mSystemBallCount = mRegister->worldConfig().settings().balls;
	BufferObjs::Weight weight
	{
		mRegister->worldConfig().settings().light,
		mRegister->worldConfig().settings().medium,
		mRegister->worldConfig().settings().heavy
	};
	SetThreadAffinityMask(GetCurrentThread(), 0x0);
	mGraphics = std::unique_ptr<Graphics::GraphicsCore>
		(new Graphics::GraphicsCore(mRegister,
			mGfxBfBalls,
			mGfxBfPeers,
			mGfxPeer,
			weight));

	
}

void
Application::initialiseSharedObjects()
{
	mGfxBfBalls = std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>>(
		new Utilities::DoubleBuffer<BufferObjs::PhyToGfx>());
	mGfxBfPeers = std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>>(
		new Utilities::DoubleBuffer<BufferObjs::PhyToGfx>());
	mGfxPeer = std::shared_ptr<Shared::Peer>(new Shared::Peer);
	mNetPeer = std::shared_ptr<Shared::Peer>(new Shared::Peer);

	mIntersystemExchange = std::shared_ptr<Shared::SharingMechanism>(
		new Shared::SharingMechanism());
}

bool
Application::run()
{
	mResetRequired = false;
	mTerminate = false;
	/*------------------------------------------------------------------------*/
	/*-------------------Graphics System Configure----------------------------*/
	/*------------------------------------------------------------------------*/
	SDL_SetRelativeMouseMode(mMouseIsCaptured ? SDL_TRUE : SDL_FALSE);
	const std::shared_ptr<Render::d3d11::D3DContext>
		context = mWindow.context();
	//configure world
	mGraphics->generateWorld();
	Render::Pass::Master rp(context, mGraphics->scene(), mRegister->shaderPaths());
	//initialise default states.
	mScene = mGraphics->scene();
	
	//init TW...
	TwInit(TW_DIRECT3D11, context->device().Get());
	auto winSize = mWindow.size();
	TwWindowSize(winSize.x, winSize.y);
	mStats = std::unique_ptr<UI::Statistics>(new UI::Statistics);

	/*------------------------------------------------------------------------*/
	/*-------------------Initialise External Config Struct--------------------*/
	/*------------------------------------------------------------------------*/

	mStats->physicsHz(mRegister->worldConfig().settings().phyHz);
	mStats->networkHz(mRegister->worldConfig().settings().netHz);
	mStats->graphicsHz(mRegister->worldConfig().settings().gfxHz);
	mStats->ballcount(mRegister->worldConfig().settings().balls);
	mStats->friction(mRegister->worldConfig().settings().friction);
	mStats->elasticity(mRegister->worldConfig().settings().elasticity);
	mStats->timescale(mRegister->worldConfig().settings().timescale);

	BufferObjs::Weight weight
	{
		mRegister->worldConfig().settings().light,
		mRegister->worldConfig().settings().medium,
		mRegister->worldConfig().settings().heavy
	};

	/*------------------------------------------------------------------------*/
	/*-------------------Program Initialisation Flow--------------------------*/
	/*------------------------------------------------------------------------*/
	
	std::atomic<bool> isMaster = false;
	std::atomic<bool> net_join_finished = false;
	mUINetRequestPause.first = false;
	mUINetRequestPause.second = false;
	mPhysicsPause = false;

	//begin network
	mNetwork = std::unique_ptr<Net::NetworkCore>
		(new Net::NetworkCore(
			mStats->networkHz(),
			isMaster,
			net_join_finished,
			mNetPeer,
			mGfxBfPeers,
			mStats->owned(),
			mUINetRequestPause,
			mPhysicsPause,
			mIntersystemExchange
		));

	//begin physics (subservient mode vs dominant node.)
	mNetwork->start();

	Physics::PhysicsCore::External px
	{
		mStats->physicsHz(),
		mStats->ballcount(),
		mStats->contested(),
		mStats->owned(),
		mStats->friction(),
		mStats->elasticity(),
		mStats->wellForce(),
		mStats->timescale(),
		mStats->timestep()
	};

	while (!net_join_finished) { ; }

	mPhysics = std::unique_ptr<Physics::PhysicsCore>
		(new Physics::PhysicsCore(
			mSystemBallCount,
			isMaster,
			mGfxBfBalls,
			mGfxPeer,
			mNetPeer,
			px,
			mPhysicsPause,
			weight,
			mIntersystemExchange
		));
	mPhysics->setFriction(mRegister->worldConfig().settings().friction);
	mPhysics->setRestitution(mRegister->worldConfig().settings().elasticity);
	mPhysics->start();

	/*------------------------------------------------------------------------*/
	/*-------------------Graphics & UI System Running-------------------------*/
	/*------------------------------------------------------------------------*/

	mGraphics->preallocateProps(mSystemBallCount);
	Utilities::Clock hzClock;
	double elapsedTime = 0;
	while (!mTerminate)
	{
		elapsedTime += hzClock.tick(); //returns answer in seconds
		//userInputHandling();
		double target = 1.0 / *mStats->graphicsHz();
		if (elapsedTime > target)
		{
			mGraphics->updateScene();

			rp.execute();
			userInputHandling();
			elapsedTime = 0.0f;
		}
			
	}
	TwTerminate();
	mPhysics->stop();
	mNetwork->stop();
	return mResetRequired;
}

void 
Application::userInputHandling()
{
	mWM.propogateEvents();
	UI_Default();
}

void
Application::UI_Default()
{
	SDL_Event event;
	while (mWindow.popEvent(&event))
	{
		TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
		if (event.type == SDL_WINDOWEVENT)
		{
			if (event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				mTerminate = true;
			}
			if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
				event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				mWindow.resized();
				TwWindowSize(mWindow.size().x, mWindow.size().y);
				mScene->setAspect(mWindow.size());
			}
		}
		else if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_BACKSPACE:
					//Override Mouse Capture.
					mMouseIsCaptured = !mMouseIsCaptured;
					SDL_SetRelativeMouseMode(mMouseIsCaptured ?
						SDL_TRUE : SDL_FALSE);
				break;
				case SDLK_ESCAPE:
					mTerminate = true;
					break;
				case SDLK_r:
					mResetRequired = true;
					mTerminate = true;
					break;
				case SDLK_p:
					mUI_State.paused = !mUI_State.paused;
					mUINetRequestPause.second = mUI_State.paused;
					mUINetRequestPause.first = true;
					break;	
				//Simulation Controls
				case SDLK_o: //phys hz >
				{
					mStats->physicsHz(*mStats->physicsHz() + 10);
					break;
				}
				case SDLK_l:  //phys hz <
				{
					auto val = *mStats->physicsHz();
					mStats->physicsHz(((val - 10) > 1)? val-10 : 1 );//cap at 1.
					break;
				}
				case SDLK_i: //gfx hz >
				{
					mStats->graphicsHz(*mStats->graphicsHz() + 10);
					break;
				}
				case SDLK_k:  //gfx hz <
				{
					auto val = *mStats->graphicsHz();
					mStats->graphicsHz(((val - 10) > 1) ? val - 10 : 1);//cap at 1.
					break;
				}
				case SDLK_u: //net hz >
				{
					mStats->networkHz(*mStats->networkHz() + 10);
					break;
				}
				case SDLK_j:  //net hz <
				{
					auto val = *mStats->networkHz();
					mStats->networkHz(((val - 10) > 1) ? val - 10 : 1);//cap at 1.
					break;
				}
				case SDLK_y: //timescale  >
				{
					auto val = *mStats->timescale();
					mStats->networkHz(((val +0.01) > 1.0) ? 1.0f : val+0.1);//cap at 1.
					break;
				}
				case SDLK_h:  //timescale  <
				{
					auto val = *mStats->timescale();
					mStats->networkHz(((val - 0.01) > 0.01) ? 0.01f : val -0.01);
					//cap at 0.01.
					break;
				}
				case SDLK_m: //well height  >
				{
					mPhysics->movePeer(glm::vec3(0, 5, 0));
					break;
				}
				case SDLK_n:  //well height <
				{
					mPhysics->movePeer(glm::vec3(0, -5, 0));
					break;
				}
				//Camera Cardinal Controls
				case SDLK_w:
				{
					mScene->camera().move(mCardinals.UP);
					break;
				}
				case SDLK_s:
				{
					mScene->camera().move(mCardinals.DOWN);
					break;
				}
				case SDLK_a:
				{
					mScene->camera().move(mCardinals.LEFT);
					break;
				}
				case SDLK_d:
				{
					mScene->camera().move(mCardinals.RIGHT);
					break;
				}
				case SDLK_UP:
				{
					mScene->camera().move(mCardinals.FORWARD);
					break;
				}
				case SDLK_DOWN:
				{
					mScene->camera().move(mCardinals.BACKWARD);
					break;
				}
				//camera orientation control.
				case SDLK_KP_8:
				{
					const auto mod = event.key.keysym.mod;
					if (mod & SDLK_LCTRL || mod & SDLK_RCTRL)
					{
						mScene->camera().move(mCardinals.UP);
					}
					else
					{
						mScene->camera().tilt(mRotation.UP);
					}
					break;
				}
				case SDLK_KP_6:
				{
					mScene->camera().pan(mRotation.LEFT);
					break;
				}			
				case SDLK_KP_2:
				{
					mScene->camera().tilt(mRotation.DOWN);
					break;
				}
				case SDLK_KP_4:
				{
					mScene->camera().pan(mRotation.RIGHT);
					break;
				}
				case SDLK_F5:
				{
					mScene->specialRenderMode(!mScene->specialRenderMode());
					break;
				}
			}
		}
		/*
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				mPhysics->gravityWellMagnitude(-1);
			}
			mUI_State.mouseDown = true;
		}*/
		/*
		if (event.type == SDL_MOUSEBUTTONUP)
		{
			mUI_State.mouseDown = false;
		}*/
		if (event.type == SDL_MOUSEMOTION /* && mUI_State.mouseDown*/)
		{
			mPhysics->movePeer(
				glm::vec3(event.motion.xrel, 0, -event.motion.yrel));
			//mGraphics->movePeer(event.motion.xrel, event.motion.yrel);
		}
	}
	auto mouseState = SDL_GetMouseState(nullptr, nullptr);
	if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
		{
			mPhysics->zeroGravityWell();
		}
		else
		{
			mPhysics->gravityWellMagnitude(-0.2);
		}
	}
	else if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		mPhysics->gravityWellMagnitude(0.2);
	}
}


/*

mWorldClock.reset();
Utilities::Clock frameRateCounter;
int currentFrames = 0,  displayFrames=0;
double seconds = 0.0;

seconds += frameRateCounter.tick();
if (seconds > 1)
{
displayFrames = currentFrames;
currentFrames = 0;
seconds = 0;
}
currentFrames++;

*/