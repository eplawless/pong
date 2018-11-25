#pragma once
#include <Windows.h>
#include <cstdint>

#include "input.h"
#include "scene.h"
#include "timer.h"

class Game
{
public: // methods
	Game(LPCWSTR applicationName);
	~Game();

	void Run();

private: // types
	enum class UpdateResult 
	{ 
		Exit, 
		Continue 
	};

private: // methods
	UpdateResult Update(int64_t usDeltaTime);
	void Render();
	bool InitializeWindow(uint32_t width, uint32_t height);
	void ShutdownWindow();
	LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private: // friends
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private: // members
	Timer m_timer;
	Input m_input;
	Scene m_scene;

	HINSTANCE m_hInstance;
	HWND m_hWindow;
	LPCWSTR m_applicationName;
};
