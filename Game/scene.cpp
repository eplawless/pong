#include "scene.h"

float const LEFT_GOAL_POSITION_X = -6.0f;
float const RIGHT_GOAL_POSITION_X = 6.0f;

float const VELOCITY_GROWTH_RATE_X = 1.1f;


Scene::Scene()
	: m_leftPaddle(-5.0f, Paddle::Side::Left)
	, m_rightPaddle(4.8f, Paddle::Side::Right)
{
}

bool Scene::Initialize(Window &window, Renderer &renderer)
{
	m_camera.SetPosition(0.0f, 0.0f, -10.0f);

	if (   !m_leftPaddle.Initialize(window, renderer) 
		|| !m_rightPaddle.Initialize(window, renderer)
		|| !m_ball.Initialize(window, renderer))
	{
		window.ShowMessageBox("Error", "Could not initialize the paddle and ball objects.");
		return false;
	}

	m_textureShader = renderer.GetTextureShader(L"Assets/Textures/Wood.gif");
	if (!m_textureShader->Initialize())
	{
		window.ShowMessageBox("Error", "Could not initialize the texture shader.");
		return false;
	}
	
	return true;
}

void Scene::Shutdown()
{
	m_textureShader->Shutdown();
	m_ball.Shutdown();
	m_leftPaddle.Shutdown();
	m_rightPaddle.Shutdown();
	m_textureShader.reset();
}

void Scene::Reset()
{
	m_leftPaddle.Reset();
	m_rightPaddle.Reset();
	m_ball.Reset();
}

void Scene::HandleEvents(PongEventList const &arrEvents)
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
	if (CollideBallWithGoals()) { Reset(); }
}

void Scene::Render(Renderer &renderer)
{
	DirectX::XMMATRIX objectToWorld = renderer.GetWorldMatrix();
	DirectX::XMMATRIX worldToView = m_camera.GetViewMatrix();
	DirectX::XMMATRIX viewToClip = renderer.GetProjectionMatrix();

	m_leftPaddle.Render(renderer, *m_textureShader, objectToWorld, worldToView, viewToClip);
	m_rightPaddle.Render(renderer, *m_textureShader, objectToWorld, worldToView, viewToClip);
	m_ball.Render(renderer, *m_textureShader, objectToWorld, worldToView, viewToClip);
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
