#include "paddle.h"

#include <Windows.h>
#include <DirectXMath.h>

float const S_PER_US = 1.0f / 1000000.0f;

Paddle::Paddle(float positionX, Side side)
	: m_side(side)
	, m_bounds{ positionX, 0.0f, 0.2f, 1.0f }
	, m_moveState(Movement::Stopped)
	, m_moveSpeedY(1.0f)
	, m_model(Model::CreateQuad(m_bounds.sizeX, m_bounds.sizeY))
{
	Reset();
}

void Paddle::Reset()
{
	m_bounds.positionY = 0.0f;
	m_moveState = Movement::Stopped;
}

bool Paddle::Initialize(
	D3D &d3d)
{
	return m_model.Initialize(d3d.GetDevice());
}

void Paddle::Shutdown()
{
	m_model.Shutdown();
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
	float deltaPositionY = S_PER_US * usdt * m_moveSpeedY;
	m_bounds.positionY += deltaPositionY * m_moveState;
}

void Paddle::Render(
	D3D &d3d,
	TextureShader &shader,
	DirectX::XMMATRIX objectToWorld,
	DirectX::XMMATRIX worldToView,
	DirectX::XMMATRIX viewToClip)
{
	ID3D11DeviceContext *pDeviceContext = d3d.GetDeviceContext();
	m_model.Render(pDeviceContext);
	shader.Render(
		pDeviceContext,
		m_model.GetIndexCount(),
		objectToWorld * DirectX::XMMatrixTranslation(m_bounds.positionX, m_bounds.positionY, 0.0f),
		worldToView,
		viewToClip,
		m_model.GetTexture()
	);
}
