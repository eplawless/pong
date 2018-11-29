#include "scene.h"

float const LEFT_GOAL_POSITION_X = -6.0f;
float const RIGHT_GOAL_POSITION_X = 6.0f;

float const VELOCITY_GROWTH_RATE_X = 1.1f;


Scene::Scene()
	: m_leftPaddle(-5.0f, Paddle::Side::Left)
	, m_rightPaddle(4.8f, Paddle::Side::Right)
{
}

bool Scene::Initialize(D3D &d3d)
{
	HWND hwnd = d3d.GetWindowHandle();
	ID3D11Device *pDevice = d3d.GetDevice();

	m_camera.SetPosition(0.0f, 0.0f, -10.0f);

	if (   !m_leftPaddle.Initialize(d3d) 
		|| !m_rightPaddle.Initialize(d3d) 
		|| !m_ball.Initialize(d3d))
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
	m_leftPaddle.Shutdown();
	m_rightPaddle.Shutdown();
}

void Scene::Reset()
{
	m_leftPaddle.Reset();
	m_rightPaddle.Reset();
	m_ball.Reset();
}

void Scene::HandleEvents(GameEventList const &arrEvents)
{
	m_leftPaddle.HandleEvents(arrEvents);
	m_rightPaddle.HandleEvents(arrEvents);
	m_ball.HandleEvents(arrEvents);
}

void Scene::Update(int64_t usDeltaTime)
{
	m_camera.Update();
	m_leftPaddle.Update(usDeltaTime);
	m_rightPaddle.Update(usDeltaTime);
	m_ball.Update(usDeltaTime);

	if (CollideBallWithPaddles()) { return; }
	if (CollideBallWithGoals())
	{
		Reset();
	}
}

void Scene::Render(D3D &d3d)
{
	DirectX::XMMATRIX objectToWorld = d3d.GetWorldMatrix();
	DirectX::XMMATRIX worldToView = m_camera.GetViewMatrix();
	DirectX::XMMATRIX viewToClip = d3d.GetProjectionMatrix();

	m_leftPaddle.Render(d3d, m_shader, objectToWorld, worldToView, viewToClip);
	m_rightPaddle.Render(d3d, m_shader, objectToWorld, worldToView, viewToClip);
	m_ball.Render(d3d, m_shader, objectToWorld, worldToView, viewToClip);
}

bool Scene::CollideBallWithPaddles()
{
	Vector2D ballVelocity = m_ball.GetVelocity();
	Box2D ballBounds = m_ball.GetBounds();
	bool hitRightPaddle = ballVelocity.x > 0 && ballBounds.Intersects(m_rightPaddle.GetBounds());
	bool hitLeftPaddle = ballVelocity.x < 0 && ballBounds.Intersects(m_leftPaddle.GetBounds());
	if (!hitRightPaddle && !hitLeftPaddle) { return false; }

	Paddle const &paddle = hitLeftPaddle ? m_leftPaddle : m_rightPaddle;
	Box2D paddleBounds = paddle.GetBounds();
	Vector2D newBallVelocity;
	newBallVelocity.x = -ballVelocity.x * VELOCITY_GROWTH_RATE_X;
	newBallVelocity.y = (ballBounds.GetCenter().y - paddleBounds.GetCenter().y) / (paddleBounds.sizeY * 0.5f);
	m_ball.SetVelocity(newBallVelocity);
	return true;
}

bool Scene::CollideBallWithGoals()
{
	Vector2D ballVelocity = m_ball.GetVelocity();
	Box2D ballBounds = m_ball.GetBounds();
	bool hitRightGoal = ballVelocity.x > 0 && ballBounds.positionX >= RIGHT_GOAL_POSITION_X;
	bool hitLeftGoal = ballVelocity.x < 0 && ballBounds.positionX <= LEFT_GOAL_POSITION_X;
	return (hitLeftGoal || hitRightGoal);
}
