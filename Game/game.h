#pragma once
#include <Windows.h>
#include <cstdint>

#include "../Engine/Core/input-mapper.h"
#include "../Engine/Core/timer.h"
#include "../Engine/Core/Window/Window.h"
#include "../Engine/Graphics/Drivers/Renderer.h"
#include "../Engine/Physics/Physics.h"

#include "scene.h"
#include "debug-overlay.h"
#include "pong-event.h"

const bool FULLSCREEN_ENABLED = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Game
{
public: // methods
	Game(Window &window, Renderer &renderer);
	~Game();

	void Run();

private: // types
	enum class LoopAction { Exit, Continue };

private: // methods
	LoopAction HandleEvents(PongEventList const &arrGameEvents);
	void Update(int64_t usDeltaTime);
	void ToggleDebugOverlay();
	void Render();
	bool Initialize();
	void ShutdownWindow();

private: // members
	Timer m_timer;
	InputMapper<PongEvent> m_inputMapper;
	Scene m_scene;
	DebugOverlay m_debugOverlay;

	Window &m_window;
	Renderer &m_renderer;
};
