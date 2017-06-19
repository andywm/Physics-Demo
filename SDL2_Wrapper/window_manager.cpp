#ifndef  _WINSOCKAPI_
#define _WINSOCKAPI_
#endif // ! #define _WINSOCKAPI_
#include "window_archtype.h"
#include "window_manager.h"
#include <SDL2/SDL_events.h>
#include<algorithm>
#include<vector>

//defaults
WindowManager * WindowManager::smSingleton=nullptr;
uint WindowManager::smSDL_Flags = SDL_INIT_EVERYTHING;

WindowManager::WindowManager()
	:mActiveWindow(nullptr)
{
	if (smSingleton == nullptr)
	{
		SDL_Init(smSDL_Flags);
		smSingleton = this;
	}
}
WindowManager::~WindowManager()
{
	if (smSingleton == this)
	{
		SDL_Quit();
		smSingleton = nullptr;
	}
}

bool 
WindowManager::addWindowRecord(WindowArchtype * const window)
{
	//don't add duplicate entries.
	const auto it = std::find(smSingleton->mWindows.begin(),
		smSingleton->mWindows.end(), window);

	if (it == smSingleton->mWindows.end())
	{
		smSingleton->mWindows.push_back(window);
		return true;
	}
	return false;
}

bool 
WindowManager::remWindowRecord(WindowArchtype * const window)
{
	const auto it = std::find(smSingleton->mWindows.begin(), 
		smSingleton->mWindows.end(), window);

	if (it != smSingleton->mWindows.end())
	{
		smSingleton->mWindows.erase(it);
		return true;
	}
	return false;
}

WindowArchtype * 
WindowManager::getWindowReference(const int id)
{
	for (WindowArchtype * const win : smSingleton->windows())
	{
		if (win->windowID() == id)
		{
			return win;
		}
	}
	return nullptr;
}

void 
WindowManager::propogateEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		int winid=0;
		//only events bound to a window can be forwarded to a window
		//TODO: add a handler for global events...
		switch(event.type)
		{
			case SDL_WINDOWEVENT:
				winid=event.window.windowID;
			break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEMOTION:
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				winid=event.key.windowID;
			break;
		};

		smSingleton->mActiveWindow = getWindowReference(winid);
		if (mActiveWindow != nullptr)
			smSingleton->mActiveWindow->pushEvent(event);
	}
}

