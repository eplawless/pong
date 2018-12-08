#include "game.h"

#include <cassert>

static Game *s_pGame = nullptr;

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

Game::Game(
	LPCWSTR applicationName)
	: m_hInstance(nullptr)
	, m_applicationName(applicationName)
	, m_inputMapper(
		InputMapper<PongEvent>::LoadConfigFromFile("input.cfg", s_eventNameToEvent))
{
	assert(s_pGame == nullptr);
	s_pGame = this;
	InitializeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
}

Game::~Game()
{
	ShutdownWindow();
	s_pGame = nullptr;
}

void Game::Run()
{
	m_input.Reset();
	m_scene.Initialize(m_d3d);

	m_timer.Start();
	int64_t usLastFrameStartTime = m_timer.GetElapsedMicroseconds();

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) { break; }

		Input::KeyEventList arrKeyEvents = m_input.GetAndClearKeyEvents();
		PongEventList arrGameEvents = m_inputMapper.MapToGameEventList(arrKeyEvents);
		if (HandleEvents(arrGameEvents) == LoopAction::Exit) { break; }

		int64_t usFrameStartTime = m_timer.GetElapsedMicroseconds();
		int64_t usDeltaTime = usFrameStartTime - usLastFrameStartTime;
		usLastFrameStartTime = usFrameStartTime;
		Update(usDeltaTime);

		Render();
	}

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

bool Game::InitializeWindow(
	uint32_t windowWidth, 
	uint32_t windowHeight)
{
	m_hInstance = GetModuleHandle(nullptr);

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

	ShowWindow(m_hWindow, SW_SHOW);
	SetForegroundWindow(m_hWindow);
	SetFocus(m_hWindow);
	ShowCursor(true);

	m_d3d.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, true, m_hWindow, false, SCREEN_DEPTH, SCREEN_NEAR);
	m_debugOverlay.Initialize(m_hWindow, m_d3d);

	return true;
}

void Game::ShutdownWindow()
{
	m_d3d.Shutdown();
	m_debugOverlay.Shutdown();
	ShowCursor(true);
	DestroyWindow(m_hWindow);
	m_hWindow = nullptr;
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = nullptr;
}

LRESULT Game::HandleMessage(
	HWND hwnd, 
	UINT message, 
	WPARAM wparam, 
	LPARAM lparam)
{
	if (m_debugOverlay.HandleMessage(hwnd, message, wparam, lparam))
	{
		return true;
	}

	switch (message)
	{
	case WM_KEYDOWN:
		m_input.SetKeyDown(static_cast<unsigned int>(wparam));
		return 0;
	case WM_KEYUP:
		m_input.SetKeyUp(static_cast<unsigned int>(wparam));
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

	if (s_pGame == nullptr) 
	{ 
		return 0; 
	}

	return s_pGame->HandleMessage(hwnd, message, wparam, lparam);
}
