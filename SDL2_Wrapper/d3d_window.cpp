#pragma once
#ifndef  _WINSOCKAPI_
#define _WINSOCKAPI_
#endif // ! #define _WINSOCKAPI_

#include "d3d_window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <memory>

D3DWindow::D3DWindow(
	const std::string & title = "SDL Window",
	const uint w = 512, const uint h = 512,
	const uint flags = 0)
	:
	WindowArchtype(title, w, h, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, flags),
	mContext(new Render::d3d11::D3DContext(getWin32Handle(), size()))
{}

HWND 
D3DWindow::getWin32Handle() const
{
	SDL_SysWMinfo systemInfo;
	SDL_VERSION(&systemInfo.version);
	SDL_GetWindowWMInfo(window(), &systemInfo);

	return systemInfo.info.win.window;
}

void
D3DWindow::resized()
{
	mContext->resize(size());
}

std::shared_ptr < Render::d3d11::D3DContext > 
D3DWindow::context() const
{
	return mContext;
}
