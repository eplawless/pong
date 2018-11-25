#include "scene.h"

Scene::Scene()
	: m_playerPaddle(-5.0f)
	, m_computerPaddle(4.8f)
{
}

bool Scene::Initialize(HWND hwnd, D3D &d3d)
{
	ID3D11Device *pDevice = d3d.GetDevice();

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
	Ball::UpdateResult result = m_ball.Update(usDeltaTime, m_playerPaddle, m_computerPaddle);
	if (   result == Ball::UpdateResult::HitLeftGoal 
		|| result == Ball::UpdateResult::HitRightGoal)
	{
		Reset();
	}
}

void Scene::Render(D3D &d3d)
{
	DirectX::XMMATRIX objectToWorld = d3d.GetWorldMatrix();
	DirectX::XMMATRIX worldToView = m_camera.GetViewMatrix();
	DirectX::XMMATRIX viewToClip = d3d.GetProjectionMatrix();
	ID3D11DeviceContext *pDeviceContext = d3d.GetDeviceContext();

	m_playerPaddle.Render(pDeviceContext, m_shader, objectToWorld, worldToView, viewToClip);
	m_computerPaddle.Render(pDeviceContext, m_shader, objectToWorld, worldToView, viewToClip);
	m_ball.Render(pDeviceContext, m_shader, objectToWorld, worldToView, viewToClip);
}
