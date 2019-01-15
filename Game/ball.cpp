#include "ball.h"

float const S_PER_US = 1.0f / 1000000.0f;

float const TOP_WALL_POSITION_Y = 3.9f;
float const BOTTOM_WALL_POSITION_Y = -4.1f;

Ball::Ball()
	: m_model(Model::CreateQuad(0.2, 0.2))
{
}

void Ball::Reset()
{
	if (!m_ball) return;
	m_ball->position.x = 0.5;
	m_ball->position.y = 0.5;
	m_ball->velocity.x = 0.5;
	m_ball->velocity.y = 0.5;
}

bool Ball::Initialize(Window &window, Renderer &renderer)
{
	m_ball.emplace(Physics::Get().CreateBall(0.5, 0.5, 0.5, 0.5, 0.1));
	return m_model.Initialize(window, renderer);
}

void Ball::Shutdown()
{
	m_model.Shutdown();
	m_ball.reset();
}

void Ball::HandleEvents(PongEventList const &arrEvents)
{
}

void Ball::Update(uint64_t usdt)
{
	if (!m_ball) return;
	Physics::BallProxy &ball = *m_ball;

	// move
	ball.position.x += S_PER_US * usdt * ball.velocity.x;
	ball.position.y += S_PER_US * usdt * ball.velocity.y;

	// hit walls
	bool hitTopWall = ball.velocity.y > 0 && (ball.position.y + ball.radius) >= TOP_WALL_POSITION_Y;
	bool hitBottomWall = ball.velocity.y < 0 && (ball.position.y - ball.radius) <= BOTTOM_WALL_POSITION_Y;
	if (hitTopWall || hitBottomWall) 
	{ 
		ball.velocity.y = -ball.velocity.y;
	}
}

void Ball::Render(
	Renderer &renderer,
	Shader &shader,
	DirectX::XMMATRIX objectToWorld,
	DirectX::XMMATRIX worldToView,
	DirectX::XMMATRIX viewToClip)
{
	if (!m_ball) return;
	Physics::BallProxy &ball = *m_ball;

	double left = ball.position.x - ball.radius;
	double top = ball.position.y + ball.radius;
	objectToWorld = objectToWorld * DirectX::XMMatrixTranslation(left, top, 0.0f);
	shader.Render(objectToWorld, worldToView, viewToClip);
	m_model.Render(renderer);
}

Vector2D Ball::GetVelocity() const
{
	return m_ball 
		? Vector2D{ m_ball->velocity.x, m_ball->velocity.y }
		: Vector2D{ 0, 0 };
}

void Ball::SetVelocity(Vector2D const &velocity)
{
	if (m_ball)
	{
		m_ball->velocity.x = velocity.x;
		m_ball->velocity.y = velocity.y;
	}
}

Box2D Ball::GetBounds() const
{
	if (!m_ball)
	{
		return { 0, 0, 0, 0 };
	}
	double left = m_ball->position.x - m_ball->radius;
	double top = m_ball->position.y + m_ball->radius;
	double size = m_ball->radius * 2.0;
	return { left, top, size, size };
}
