#include "Win32Window.h"

#include <cassert>

static Win32Window *s_pWindow = nullptr;

Win32Window::Win32Window(LPCWSTR name, uint32_t width, uint32_t height)
	: m_hInstance(GetModuleHandle(nullptr))
	, m_hWindow(nullptr)
	, m_applicationName(name)
	, Window(width, height)
{
	assert(s_pWindow == nullptr);
	s_pWindow = this;

	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);
	DWORD error = GetLastError();

	uint32_t windowWidth = GetWidth();
	uint32_t windowHeight = GetHeight();
	uint32_t screenWidth = GetSystemMetrics(SM_CXSCREEN);
	uint32_t screenHeight = GetSystemMetrics(SM_CYSCREEN);
	uint32_t windowPositionX = (screenWidth - windowWidth) / 2;
	uint32_t windowPositionY = (screenHeight - windowHeight) / 2;

	m_hWindow = CreateWindowEx(
		WS_EX_APPWINDOW,
		wc.lpszClassName,
		m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		windowPositionX, windowPositionY,
		windowWidth, windowHeight,
		nullptr,
		nullptr,
		m_hInstance,
		nullptr);
}

Win32Window::~Win32Window()
{
	s_pWindow = nullptr;
	DestroyWindow(m_hWindow);
	m_hWindow = nullptr;
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = nullptr;
}

void Win32Window::Show()
{
	ShowWindow(m_hWindow, SW_SHOW);
	SetForegroundWindow(m_hWindow);
	SetFocus(m_hWindow);
}

void Win32Window::Hide()
{
	ShowWindow(m_hWindow, SW_HIDE);
}

void Win32Window::PumpMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Win32Window::ShowMessageBox(std::string const &title, std::string const &message)
{
	MessageBoxA(m_hWindow, title.c_str(), message.c_str(), MB_OK);
}

void Win32Window::ShowMessageBox(std::wstring const &title, std::wstring const &message)
{
	MessageBox(m_hWindow, title.c_str(), message.c_str(), MB_OK);
}

LRESULT Win32Window::HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		m_input.SetKeyDown(static_cast<unsigned int>(wparam));
		return 0;
	case WM_KEYUP:
		m_input.SetKeyUp(static_cast<unsigned int>(wparam));
		return 0;
	case WM_QUIT:
		for (auto handleListenerPair : m_quitEventListenerByHandle)
		{
			handleListenerPair.second(static_cast<int>(lparam));
		}
		return 0;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
}

LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	if (message == WM_DESTROY || message == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;
	}

	if (s_pWindow != nullptr)
	{
		return s_pWindow->HandleMessage(hwnd, message, wparam, lparam);
	}

	return 0;
}
