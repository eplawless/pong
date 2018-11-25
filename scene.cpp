#include "scene.h"

Scene::Scene()
	: m_playerPaddle(-5.0f)
	, m_computerPaddle(4.8f)
{
}

bool Scene::Initialize(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd)
{
	m_d3d.Initialize(screenWidth, screenHeight, true, hwnd, false, SCREEN_DEPTH, SCREEN_NEAR);

	ID3D11Device *pDevice = m_d3d.GetDevice();

	m_camera.SetPosition(0.0f, 0.0f, -10.0f);

	if (   !m_playerPaddle.Initialize(pDevice) 
		|| !m_computerPaddle.Initialize(pDevice) 
		|| !m_ball.Initialize(pDevice))
	{
		MessageBox(hwnd, TEXT("Could not initialize the paddle and ball objects."), TEXT("Error"), MB_OK);
		return false;
	}

	if (!m_shader.Initialize(pDevice, hwnd))
	{
		MessageBox(hwnd, TEXT("Could not initialize the shader object."), TEXT("Error"), MB_OK);
		return false;
	}
	
	return true;
}

void Scene::Shutdown()
{
	m_shader.Shutdown();
	m_ball.Shutdown();
	m_playerPaddle.Shutdown();
	m_computerPaddle.Shutdown();
	m_d3d.Shutdown();
}

void Scene::Reset()
{
	m_playerPaddle.Reset();
	m_computerPaddle.Reset();
	m_ball.Reset();
}

void Scene::Update(
	int64_t usDeltaTime,
	Input const &input)
{
	m_camera.Update();
	m_playerPaddle.Update(usDeltaTime, input);
	m_computerPaddle.Update(usDeltaTime, input);
	switch (m_ball.Update(usDeltaTime, m_playerPaddle, m_computerPaddle))
	{
	case Ball::UpdateResult::HitLeftGoal: // passthrough
	case Ball::UpdateResult::HitRightGoal:
		Reset();
		break;
	default:
		break;
	}
}

void Scene::Render()
{
	DirectX::XMMATRIX worldToView = m_camera.GetViewMatrix();
	DirectX::XMMATRIX objectToWorld = m_d3d.GetWorldMatrix();
	DirectX::XMMATRIX viewToClip = m_d3d.GetProjectionMatrix();
	ID3D11DeviceContext *pDeviceContext = m_d3d.GetDeviceContext();

	m_d3d.BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	m_playerPaddle.Render(pDeviceContext, m_shader, objectToWorld, worldToView, viewToClip);
	m_computerPaddle.Render(pDeviceContext, m_shader, objectToWorld, worldToView, viewToClip);
	m_ball.Render(pDeviceContext, m_shader, objectToWorld, worldToView, viewToClip);
	m_d3d.EndScene();
}
