#include "paddle.h"

#include <Windows.h>

float const S_PER_US = 1.0f / 1000000.0f;

Paddle::Paddle(float positionX)
	: m_bounds{ positionX, 0.0f, 0.2f, 1.0f }
	, m_moveSpeedY(1.0f)
	, m_model(Model::CreateQuad(m_bounds.sizeX, m_bounds.sizeY))
{
}

bool Paddle::Initialize(
	ID3D11Device *pDevice)
{
	return m_model.Initialize(pDevice);
}

void Paddle::Shutdown()
{
	m_model.Shutdown();
}

void Paddle::Update(
	uint64_t usdt,
	Input const &input)
{
	float deltaPositionY = S_PER_US * usdt * m_moveSpeedY;
	float movementUp = input.IsKeyDown(VK_UP) * deltaPositionY;
	float movementDown = input.IsKeyDown(VK_DOWN) * deltaPositionY;
	m_bounds.positionY += movementUp - movementDown;
}

void Paddle::Render(
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
