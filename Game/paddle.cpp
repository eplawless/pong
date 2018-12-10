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
	Physics::Box &box = Physics::Get().GetBox(m_hBox);
	box.position.y = 0.0;
	m_moveState = Movement::Stopped;
}

bool Paddle::Initialize(
	Window &window,
	Renderer &renderer)
{
	m_hBox = Physics::Get().CreateBox(m_defaultPositionX, 0.0, 0.2, 1.0);
	return m_model.Initialize(window, renderer);
}

void Paddle::Shutdown()
{
	m_model.Shutdown();
	Physics::Get().DestroyBox(m_hBox);
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
	Physics::Box &box = Physics::Get().GetBox(m_hBox);
	box.velocity.y = m_moveState * m_moveSpeed;
	box.position.y += S_PER_US * usdt * box.velocity.y;
}

void Paddle::Render(
	Renderer &renderer,
	Shader &shader,
	DirectX::XMMATRIX objectToWorld,
	DirectX::XMMATRIX worldToView,
	DirectX::XMMATRIX viewToClip)
{
	Physics::Box &box = Physics::Get().GetBox(m_hBox);
	objectToWorld = objectToWorld * DirectX::XMMatrixTranslation(box.position.x, box.position.y, 0.0f);
	shader.Render(objectToWorld, worldToView, viewToClip);
	m_model.Render(renderer);
}

Box2D Paddle::GetBounds() const
{
	Physics::Box &box = Physics::Get().GetBox(m_hBox);
	return { box.position.x, box.position.y, box.size.x, box.size.y };
}
