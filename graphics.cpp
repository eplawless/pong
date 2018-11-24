#include "graphics.h"

Graphics::Graphics()
	: m_playerPaddle(-5.0f)
	, m_computerPaddle(4.9f)
{
}

bool Graphics::Initialize(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd)
{
	m_d3d.Initialize(
		screenWidth,
		screenHeight,
		true,
		hwnd,
		false,
		SCREEN_DEPTH,
		SCREEN_NEAR);

	ID3D11Device *pDevice = m_d3d.GetDevice();

	m_camera.SetPosition(0.0f, 0.0f, -10.0f);
	if (!m_playerPaddle.Initialize(pDevice) || !m_computerPaddle.Initialize(pDevice))
	{
		MessageBox(hwnd, TEXT("Could not initialize the paddle objects."), TEXT("Error"), MB_OK);
		return false;
	}

	if (!m_shader.Initialize(pDevice, hwnd))
	{
		MessageBox(hwnd, TEXT("Could not initialize the shader object."), TEXT("Error"), MB_OK);
		return false;
	}
	
	return true;
}

void Graphics::Shutdown()
{
	m_shader.Shutdown();
	m_playerPaddle.Shutdown();
	m_computerPaddle.Shutdown();
	m_d3d.Shutdown();
}

void Graphics::Update(
	int64_t usDeltaTime,
	Input const &input)
{
	m_camera.Update();
	m_playerPaddle.Update(usDeltaTime, input);
	m_computerPaddle.Update(usDeltaTime, input);

	DirectX::XMMATRIX worldToView = m_camera.GetViewMatrix();
	DirectX::XMMATRIX objectToWorld = m_d3d.GetWorldMatrix();
	DirectX::XMMATRIX viewToClip = m_d3d.GetProjectionMatrix();

	m_d3d.BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	ID3D11DeviceContext *pDeviceContext = m_d3d.GetDeviceContext();
	m_playerPaddle.Render(pDeviceContext, m_shader, objectToWorld, worldToView, viewToClip);
	m_computerPaddle.Render(pDeviceContext, m_shader, objectToWorld, worldToView, viewToClip);
	m_d3d.EndScene();
}
