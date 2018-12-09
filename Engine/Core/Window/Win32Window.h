#pragma once

#include <Windows.h>

#include "Window.h"

class Win32Window : public Window
{
public: // methods
	Win32Window(LPCWSTR name, uint32_t width, uint32_t height);
	~Win32Window();

	virtual void Show() override;
	virtual void Hide() override;
	virtual void PumpMessages() override;

	HWND GetHandle() const { return m_hWindow; }

private: // methods
	LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private: // friends
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private: // members
	HINSTANCE m_hInstance;
	HWND m_hWindow;
	LPCWSTR m_applicationName;
};
