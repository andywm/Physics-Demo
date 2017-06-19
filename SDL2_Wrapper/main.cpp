#ifndef  _WINSOCKAPI_
#define _WINSOCKAPI_
#endif // ! #define _WINSOCKAPI_
#include <string>
#include <WinSock2.h>
#include "simulation.h"

//#define SDL_MAIN_HANDLED
//#undef main

int
main(int, char* [])
{
	const std::string ASSETS = "SimulationAssets";
	//only exits if run_simulation rtns false
	bool restart = false;
	do
	{
		Application sim(ASSETS);
		restart = sim.run();
	} while (restart);

	return 0;
}

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif

#ifndef GLM_LEFT_HANDED
#define GLM_LEFT_HANDED
#endif

#if defined(_MSC_VER)
#  pragma comment(lib, "d3d11.lib")
#  pragma comment(lib, "SDL2.lib")
#  pragma comment(lib, "SDL2main.lib")
#  pragma comment(lib, "tinyobjloader.lib")
#  pragma comment(lib, "AntTweakBar.lib")
#  pragma comment(lib, "Ws2_32.lib")

#  pragma comment( linker, "/subsystem:console" )
#endif
