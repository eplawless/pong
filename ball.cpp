#include "ball.h"

float const S_PER_US = 1.0f / 1000000.0f;

float const TOP_WALL_POSITION_Y = 3.9f;
float const BOTTOM_WALL_POSITION_Y = -4.1f;
float const LEFT_GOAL_POSITION_X = -6.0f;
float const RIGHT_GOAL_POSITION_X = 6.0f;

float const VELOCITY_GROWTH_RATE_X = 1.1f;

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

bool Ball::Initialize(ID3D11Device *pDevice)
{
	return m_model.Initialize(pDevice);
}

void Ball::Shutdown()
{
	m_model.Shutdown();
}

Ball::UpdateResult Ball::Update(
	uint64_t usdt, 
	Paddle const &leftPaddle, 
	Paddle const &rightPaddle)
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
		return UpdateResult::HitWall;
	}

	// hit paddles
	bool hitRightPaddle = m_velocityX > 0 && m_bounds.Intersects(rightPaddle.GetBounds());
	bool hitLeftPaddle = m_velocityX < 0 && m_bounds.Intersects(leftPaddle.GetBounds());
	if (hitRightPaddle || hitLeftPaddle)
	{
		m_velocityX = -m_velocityX * VELOCITY_GROWTH_RATE_X;

		// reflect angle
		Paddle const &paddle = hitLeftPaddle ? leftPaddle : rightPaddle;
		Box2D paddleBounds = paddle.GetBounds();
		m_velocityY = (m_bounds.GetCenter().y - paddleBounds.GetCenter().y) / (paddleBounds.sizeY * 0.5f);
		return UpdateResult::HitPaddle;
	}

	// hit goals
	bool hitRightGoal = m_velocityX > 0 && m_bounds.positionX >= RIGHT_GOAL_POSITION_X;
	bool hitLeftGoal = m_velocityX < 0 && m_bounds.positionX <= LEFT_GOAL_POSITION_X;
	if (hitLeftGoal || hitRightGoal)
	{
		return hitLeftGoal ? UpdateResult::HitLeftGoal : UpdateResult::HitRightGoal;
	}

	return UpdateResult::Moved;
}

void Ball::Render(
	ID3D11DeviceContext *pDeviceContext, 
	TextureShader &shader, 
	DirectX::XMMATRIX objectToWorld, 
	DirectX::XMMATRIX worldToView, 
	DirectX::XMMATRIX viewToClip)
{
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
