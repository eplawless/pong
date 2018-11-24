#include "paddle.h"

#include <Windows.h>

float const S_PER_US = 1.0f / 1000000.0f;

Paddle::Paddle(float positionX)
	: m_positionInWorldX(positionX)
	, m_positionInWorldY(-1.0f)
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
	m_positionInWorldY += input.IsKeyDown(VK_UP) * deltaPositionY - input.IsKeyDown(VK_DOWN) * deltaPositionY;
}

void Paddle::Render(
	ID3D11DeviceContext *pDeviceContext,
	TextureShader &shader,
	DirectX::XMMATRIX objectToWorld,
	DirectX::XMMATRIX worldToView,
	DirectX::XMMATRIX viewToClip)
{
	m_model.Render(pDeviceContext);
	ID3D11ShaderResourceView *pTexture = m_model.GetTexture();

	DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(m_positionInWorldX, m_positionInWorldY, 0.0f);
	shader.Render(
		pDeviceContext,
		m_model.GetIndexCount(),
		objectToWorld * translate,
		worldToView,
		viewToClip,
		pTexture
	);
}
