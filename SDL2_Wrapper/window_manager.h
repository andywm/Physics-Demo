#pragma once

#include "window_archtype.h"
#include<vector>

class WindowManager
{
private:
	static WindowManager * smSingleton;
	static uint smSDL_Flags;
	std::vector<WindowArchtype *> mWindows;
	inline std::vector<WindowArchtype *> &
	windows()
	{
		return mWindows;
	}
	WindowArchtype * mActiveWindow;
	WindowArchtype * getWindowReference(const int id);
public:
	WindowManager();
	WindowManager& operator=(const WindowManager&) = delete;
	WindowManager(const WindowManager &) = delete;
	~WindowManager();
	static inline void
	set_sdl_initialise_flags(const uint flags)
	{
		smSDL_Flags = flags;
	}

	void propogateEvents();
	bool addWindowRecord(WindowArchtype * const window);
	bool remWindowRecord(WindowArchtype * const window);

};