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
	Physics::Circle &circle = Physics::Get().GetCircle(m_hCircle);
	circle.position.x = 0.5;
	circle.position.y = 0.5;
	circle.velocity.x = 0.5;
	circle.velocity.y = 0.5;
}

bool Ball::Initialize(Window &window, Renderer &renderer)
{
	m_hCircle = Physics::Get().CreateCircle(0.5, 0.5, 0.5, 0.5, 0.1);
	return m_model.Initialize(window, renderer);
}

void Ball::Shutdown()
{
	m_model.Shutdown();
	Physics::Get().DestroyCircle(m_hCircle);
}

void Ball::HandleEvents(PongEventList const &arrEvents)
{
}

void Ball::Update(uint64_t usdt)
{
	// move
	Physics::Circle &circle = Physics::Get().GetCircle(m_hCircle);
	circle.position.x += S_PER_US * usdt * circle.velocity.x;
	circle.position.y += S_PER_US * usdt * circle.velocity.y;

	// hit walls
	bool hitTopWall = circle.velocity.y > 0 && (circle.position.y + circle.radius) >= TOP_WALL_POSITION_Y;
	bool hitBottomWall = circle.velocity.y < 0 && (circle.position.y - circle.radius) <= BOTTOM_WALL_POSITION_Y;
	if (hitTopWall || hitBottomWall) 
	{ 
		circle.velocity.y = -circle.velocity.y;
	}
}

void Ball::Render(
	Renderer &renderer,
	Shader &shader,
	DirectX::XMMATRIX objectToWorld,
	DirectX::XMMATRIX worldToView,
	DirectX::XMMATRIX viewToClip)
{
	Physics::Circle &circle = Physics::Get().GetCircle(m_hCircle);
	double left = circle.position.x - circle.radius;
	double top = circle.position.y + circle.radius;
	objectToWorld = objectToWorld * DirectX::XMMatrixTranslation(left, top, 0.0f);
	shader.Render(objectToWorld, worldToView, viewToClip);
	m_model.Render(renderer);
}

Vector2D Ball::GetVelocity() const
{
	Physics::Circle &circle = Physics::Get().GetCircle(m_hCircle);
	return Vector2D{ circle.velocity.x, circle.velocity.y };
}

void Ball::SetVelocity(Vector2D const &velocity)
{
	Physics::Circle &circle = Physics::Get().GetCircle(m_hCircle);
	circle.velocity.x = velocity.x; 
	circle.velocity.y = velocity.y;
}

Box2D Ball::GetBounds() const
{
	Physics::Circle &circle = Physics::Get().GetCircle(m_hCircle);
	double left = circle.position.x - circle.radius;
	double top = circle.position.y + circle.radius;
	double size = circle.radius * 2.0;
	return { left, top, size, size };
}
