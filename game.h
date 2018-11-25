#pragma once
#include <Windows.h>
#include <cstdint>

#include "d3d.h"
#include "input.h"
#include "scene.h"
#include "timer.h"
#include "debug-overlay.h"

const bool FULLSCREEN_ENABLED = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

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
	D3D m_d3d;
	Timer m_timer;
	Input m_input;
	Scene m_scene;
	DebugOverlay m_debugOverlay;

	HINSTANCE m_hInstance;
	HWND m_hWindow;
	LPCWSTR m_applicationName;
};
