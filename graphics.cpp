#include "graphics.h"

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
	if (!m_model.Initialize(pDevice))
	{
		MessageBox(hwnd, TEXT("Could not initialize the model object."), TEXT("Error"), MB_OK);
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
	m_model.Shutdown();
	m_d3d.Shutdown();
}

void Graphics::Update(int64_t usDeltaTime)
{
	m_d3d.BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	m_camera.Render();
	DirectX::XMMATRIX viewMatrix = m_camera.GetViewMatrix();
	DirectX::XMMATRIX worldMatrix = m_d3d.GetWorldMatrix();
	DirectX::XMMATRIX projectionMatrix = m_d3d.GetProjectionMatrix();
	ID3D11DeviceContext *pDeviceContext = m_d3d.GetDeviceContext();
	m_model.Render(pDeviceContext);
	ID3D11ShaderResourceView *pTexture = m_model.GetTexture();
	m_shader.Render(
		pDeviceContext, 
		m_model.GetIndexCount(), 
		worldMatrix, 
		viewMatrix, 
		projectionMatrix, 
		pTexture
	);
	m_d3d.EndScene();
}
