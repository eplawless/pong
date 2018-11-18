#include "graphics.h"

void Graphics::Initialize(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd)
{
	m_d3d.Initialize(
		screenWidth,
		screenHeight,
		true,
		hwnd,
		false,
		SCREEN_DEPTH,
		SCREEN_NEAR);
}

void Graphics::Shutdown()
{
	m_d3d.Shutdown();
}

void Graphics::Update(int64_t usDeltaTime)
{
	m_d3d.BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	m_d3d.EndScene();
}
