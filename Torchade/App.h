#pragma once
#include "Window.h"

class App {
public:
	App();
	int Go();
private:
	void Update();
private:
	Window wnd;
};
