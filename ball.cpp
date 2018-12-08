#include "ball.h"

float const S_PER_US = 1.0f / 1000000.0f;

float const TOP_WALL_POSITION_Y = 3.9f;
float const BOTTOM_WALL_POSITION_Y = -4.1f;

Ball::Ball()
	: m_bounds{ 0.5f, 0.5f, 0.2f, 0.2f }
	, m_model(Model::CreateQuad(m_bounds.sizeX, m_bounds.sizeY))
{
	Reset();
}

void Ball::Reset()
{
	m_bounds.positionX = 0.5f;
	m_bounds.positionY = 0.5f;
	m_velocityX = 0.5f;
	m_velocityY = 0.5f;
}

bool Ball::Initialize(D3D &d3d)
{
	return m_model.Initialize(d3d.GetDevice());
}

void Ball::Shutdown()
{
	m_model.Shutdown();
}

void Ball::HandleEvents(PongEventList const &arrEvents)
{
}

void Ball::Update(uint64_t usdt)
{
	// move
	m_bounds.positionX += S_PER_US * usdt * m_velocityX;
	m_bounds.positionY += S_PER_US * usdt * m_velocityY;

	// hit walls
	bool hitTopWall = m_velocityY > 0 && m_bounds.positionY >= TOP_WALL_POSITION_Y;
	bool hitBottomWall = m_velocityY < 0 && m_bounds.positionY <= BOTTOM_WALL_POSITION_Y;
	if (hitTopWall || hitBottomWall) 
	{ 
		m_velocityY = -m_velocityY;
	}
}

void Ball::Render(
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

Vector2D Ball::GetVelocity() const
{
	return Vector2D{ m_velocityX, m_velocityY };
}

void Ball::SetVelocity(Vector2D const &velocity)
{
	m_velocityX = velocity.x; 
	m_velocityY = velocity.y;
}
