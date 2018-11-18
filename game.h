#pragma once
#include <Windows.h>
#include <cstdint>

#include "input.h"
#include "graphics.h"
#include "timer.h"

class Game
{
public: // methods
	Game(LPCWSTR applicationName);
	~Game();

	void Run();

private: // types
	enum class UpdateResult { Exit, Continue };

private: // methods
	UpdateResult Update(int64_t usDeltaTime);
	bool InitializeWindow(uint32_t width, uint32_t height);
	void ShutdownWindow();
	LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private: // friends
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private: // members
	Timer m_timer;
	Input m_input;
	Graphics m_graphics;

	HINSTANCE m_hInstance;
	HWND m_hWindow;
	LPCWSTR m_applicationName;
};
