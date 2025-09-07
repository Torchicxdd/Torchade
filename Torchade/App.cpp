#include "App.h"

App::App()
	:
	wnd(800, 600, L"Torchade")
{}

int App::Go() {
    while (true) {
        if (const auto exitCode = Window::ProcessMessages()) {
            return *exitCode;
        }
        Update();
    }
}

void App::Update() {
    const float c = sin(timer.Peek()) / 2.0f + 0.5f;
    wnd.GetGraphics().ClearBuffer(c, c, 1.0f);
    wnd.GetGraphics().EndFrame();
}