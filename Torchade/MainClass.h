#pragma once

#include "Windows.h"

class MainClass
{
public:
	HRESULT CreateDesktopWindow();

	HWND GetWindowHandle() { return m_hWnd; };

private:
	HMENU m_hMenu;
	RECT m_rc;
	HWND m_hWnd;
};