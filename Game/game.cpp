#include "game.h"

#include <cassert>

static const uint32_t WINDOW_WIDTH = 800;
static const uint32_t WINDOW_HEIGHT = 600;

static InputMapper<PongEvent>::EventNameToGameEventMap s_eventNameToEvent = {
	{ "None", PongEvent::None },
	{ "LeftPaddleMoveUp", PongEvent::LeftPaddleMoveUp },
	{ "LeftPaddleMoveDown", PongEvent::LeftPaddleMoveDown },
	{ "LeftPaddleStop", PongEvent::LeftPaddleStop },
	{ "RightPaddleMoveUp", PongEvent::RightPaddleMoveUp },
	{ "RightPaddleMoveDown", PongEvent::RightPaddleMoveDown },
	{ "RightPaddleStop", PongEvent::RightPaddleStop },
	{ "Pause", PongEvent::Pause },
	{ "Quit", PongEvent::Quit },
	{ "ToggleDebugOverlay", PongEvent::ToggleDebugOverlay }
};

Game::Game(Window *pWindow, HWND hwnd)
	: m_inputMapper(InputMapper<PongEvent>::LoadConfigFromFile("Config/input.cfg", s_eventNameToEvent))
	, m_pWindow(pWindow)
	, m_hWindow(hwnd)
{
	assert(m_pWindow != nullptr);
	Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

Game::~Game()
{
	ShutdownWindow();
}

void Game::Run()
{
	m_pWindow->Show();
	m_pWindow->GetInput().Reset();
	m_scene.Initialize(m_d3d);

	m_timer.Start();
	int64_t usLastFrameStartTime = m_timer.GetElapsedMicroseconds();

	bool shouldKeepRunning = true;
	Window::EventListenerHandle quitListenerHandle = 
		m_pWindow->AddQuitEventListener([&](int) { shouldKeepRunning = false; });

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (shouldKeepRunning)
	{
		m_pWindow->PumpMessages();
		Input::KeyEventList arrKeyEvents = m_pWindow->GetInput().GetAndClearKeyEvents();
		PongEventList arrGameEvents = m_inputMapper.MapToGameEventList(arrKeyEvents);
		if (HandleEvents(arrGameEvents) == LoopAction::Exit) { break; }

		int64_t usFrameStartTime = m_timer.GetElapsedMicroseconds();
		int64_t usDeltaTime = usFrameStartTime - usLastFrameStartTime;
		usLastFrameStartTime = usFrameStartTime;
		Update(usDeltaTime);

		Render();
	}

	// TODO: exception safety?
	m_pWindow->RemoveEventListener(quitListenerHandle);

	m_scene.Shutdown();
}

Game::LoopAction Game::HandleEvents(PongEventList const &arrGameEvents)
{
	for (PongEvent const &event : arrGameEvents)
	{
		if (event == PongEvent::Quit) { return LoopAction::Exit; }
		if (event == PongEvent::ToggleDebugOverlay) { ToggleDebugOverlay(); }
		// TODO: pause here
	}
	if (!m_debugOverlay.GetOptions().isPaused)
	{
		m_scene.HandleEvents(arrGameEvents);
	}
	return LoopAction::Continue;
}

void Game::Update(int64_t usDeltaTime)
{
	if (!m_debugOverlay.GetOptions().isPaused)
	{
		m_scene.Update(usDeltaTime);
	}
}

void Game::ToggleDebugOverlay()
{
	m_debugOverlay.SetVisible(!m_debugOverlay.IsVisible());
}

void Game::Render()
{
	m_d3d.BeginScene(0.3f, 0.3f, 0.3f, 1.0f);
	m_scene.Render(m_d3d);
	m_debugOverlay.Render();
	m_d3d.EndScene();
}

bool Game::Initialize(
	uint32_t windowWidth, 
	uint32_t windowHeight)
{
	m_d3d.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, true, m_hWindow, false, SCREEN_DEPTH, SCREEN_NEAR);
	m_debugOverlay.Initialize(m_hWindow, m_d3d);

	return true;
}

void Game::ShutdownWindow()
{
	m_d3d.Shutdown();
	m_debugOverlay.Shutdown();
}
