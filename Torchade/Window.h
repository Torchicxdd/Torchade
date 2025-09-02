#pragma once
#include "framework.h"
#include "TorchadeException.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
public:
	class Exception : public TorchadeException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
private:
	class WindowClass
	{
	public:
		static const WCHAR* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const WCHAR* wndClassName = L"Torchade Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const WCHAR* name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string title);
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
private:
	int width;
	int height;
	HWND hWnd;
};

// Error exception helper macro
#define TORCHWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr)
#define TORCHWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())