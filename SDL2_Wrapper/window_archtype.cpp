#ifndef  _WINSOCKAPI_
#define _WINSOCKAPI_
#endif // ! #define _WINSOCKAPI_
#include<SDL2/SDL.h>

#include "window_archtype.h"
#include "window_manager.h"
#include<string>

//Window::Window() : mWindow(nullptr), mWindowID(-1) {}

WindowArchtype::WindowArchtype(const std::string & title = "SDL Window",
	const uint w = 512, const uint h = 512,
	const uint x = SDL_WINDOWPOS_CENTERED,
	const uint y = SDL_WINDOWPOS_CENTERED,
	const uint flags = 0)
: mWindow(nullptr), mWindowID(-1)
{
	createWindow(title, w, h, x, y, flags);
	WindowManager wm;
	wm.addWindowRecord(this);
}

WindowArchtype::~WindowArchtype()
{
	try
	{
		WindowManager wm;
		wm.remWindowRecord(this);
		if (mWindow != nullptr)
			SDL_DestroyWindow(mWindow);
	}
	catch(std::exception &)
	{
		
	}
}

bool 
WindowArchtype::createWindow(const std::string & title,
						const uint w, const uint h,
						const uint x,
						const uint y,
						const uint flags)
{
	if (mWindow != nullptr) return false;

	mWindow = SDL_CreateWindow(title.c_str(), x,y, w, h, SDL_WINDOW_RESIZABLE | flags);
	if(mWindow !=nullptr)
	{
		mWindowID = SDL_GetWindowID(mWindow);
		return true;
	}

	return false;
}



void 
WindowArchtype::pushEvent(const SDL_Event & event)
{
	mWindowEvents.push(event);
}

bool 
WindowArchtype::peekEvent(SDL_Event * const event)
{
	if (mWindowEvents.size() > 0)
	{
		*event = mWindowEvents.front();
		return true;
	}
	return false;
}

bool 
WindowArchtype::popEvent(SDL_Event * const event)
{
	if (mWindowEvents.size() > 0)
	{		*event = mWindowEvents.front();
		mWindowEvents.pop();
		return true;
	}
	return false;
}

void 
WindowArchtype::resize(const glm::ivec2 & size)
{
	SDL_SetWindowSize(mWindow, size.x, size.y);
	resized(); //propogate update to anything dependant on window size.
}

