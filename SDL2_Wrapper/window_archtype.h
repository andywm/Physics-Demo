#pragma once
#ifndef  _WINSOCKAPI_
#define _WINSOCKAPI_
#endif // ! #define _WINSOCKAPI_
#include<SDL2/SDL.h>
#include<string>
#include<queue>
#include<list>
#include<memory>
#include <glm\vec2.hpp>
using uint = unsigned int;


class WindowArchtype
{
private:
	int mWindowID;
	SDL_Window * mWindow;
	std::queue<SDL_Event> mWindowEvents;

public:
	//wrapper for a list of unique_ptrs<windows>

	//Window();
	WindowArchtype(const std::string & title,
		const uint w, const uint h,
		const uint x, const uint y,
		const uint flags);
	WindowArchtype& operator=(const WindowArchtype&) = delete;
	WindowArchtype(const WindowArchtype &) = delete;
	virtual ~WindowArchtype();
	void pushEvent(const SDL_Event & event);
	bool peekEvent(SDL_Event * const event);
	bool popEvent(SDL_Event * const event);

	inline const glm::ivec2
	WindowArchtype::size() const
	{
		int w, h;
		SDL_GetWindowSize(mWindow, &w, &h);
		return glm::ivec2(w, h);
	}
	virtual void resized()=0;
	virtual void resize(const glm::ivec2 & size);

	inline const int
	windowID() const
	{
		return mWindowID;
	}

	inline SDL_Window * const
	window() const
	{
		return mWindow;
	}

protected:
	bool createWindow(const std::string & title,
		const uint w, const uint h,
		const uint x, const uint y,
		const uint flags);
};

using win_sptr = std::unique_ptr<WindowArchtype>;
using window_group = std::vector<win_sptr>;
