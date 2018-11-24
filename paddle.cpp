#include "paddle.h"

#include <Windows.h>

float const S_PER_US = 1.0f / 1000000.0f;

Paddle::Paddle(float positionX)
	: m_positionX(positionX)
	, m_positionY(0.0f)
	, m_moveSpeedY(1.0f)
	, m_height(1.0f)
	, m_model(Model::CreateQuad(0.2f, m_height))
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
	m_positionY += movementUp - movementDown;
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
		objectToWorld * DirectX::XMMatrixTranslation(m_positionX, m_positionY, 0.0f),
		worldToView,
		viewToClip,
		m_model.GetTexture()
	);
}
