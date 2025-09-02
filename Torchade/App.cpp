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

}