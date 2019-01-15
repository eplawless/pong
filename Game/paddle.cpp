#include "paddle.h"

#include <Windows.h>
#include <DirectXMath.h>

float const S_PER_US = 1.0f / 1000000.0f;

Paddle::Paddle(float positionX, Side side)
	: m_side(side)
	, m_defaultPositionX(positionX)
	, m_moveSpeed(1.0)
	, m_moveState(Movement::Stopped)
	, m_model(Model::CreateQuad(0.2f, 1.0f))
{
}

void Paddle::Reset()
{
	if (m_box) { m_box->position.y = 0.0; }
	m_moveState = Movement::Stopped;
}

bool Paddle::Initialize(
	Window &window,
	Renderer &renderer)
{
	m_box.emplace(Physics::Get().CreateBox(m_defaultPositionX, 0.0, 0.2, 1.0));
	return m_model.Initialize(window, renderer);
}

void Paddle::Shutdown()
{
	m_model.Shutdown();
	m_box.reset();
}

void Paddle::HandleEvents(PongEventList const &arrEvents)
{
	for (PongEvent const &event : arrEvents)
	{
		if (m_side == Side::Left && event == PongEvent::LeftPaddleMoveUp) { m_moveState = Up; }
		if (m_side == Side::Left && event == PongEvent::LeftPaddleMoveDown) { m_moveState = Down; }
		if (m_side == Side::Left && event == PongEvent::LeftPaddleStop) { m_moveState = Stopped; }
		if (m_side == Side::Right && event == PongEvent::RightPaddleMoveUp) { m_moveState = Up; }
		if (m_side == Side::Right && event == PongEvent::RightPaddleMoveDown) { m_moveState = Down; }
		if (m_side == Side::Right && event == PongEvent::RightPaddleStop) { m_moveState = Stopped; }
	}
}

void Paddle::Update(uint64_t usdt)
{
	if (!m_box) return;
	m_box->velocity.y = m_moveState * m_moveSpeed;
	m_box->position.y += S_PER_US * usdt * m_box->velocity.y;
}

void Paddle::Render(
	Renderer &renderer,
	Shader &shader,
	DirectX::XMMATRIX objectToWorld,
	DirectX::XMMATRIX worldToView,
	DirectX::XMMATRIX viewToClip)
{
	if (!m_box) return;
	objectToWorld = objectToWorld * DirectX::XMMatrixTranslation(m_box->position.x, m_box->position.y, 0.0f);
	shader.Render(objectToWorld, worldToView, viewToClip);
	m_model.Render(renderer);
}

Box2D Paddle::GetBounds() const
{
	return m_box 
		? Box2D{ m_box->position.x, m_box->position.y, m_box->size.x, m_box->size.y }
		: Box2D{ 0, 0, 0, 0 };
}
