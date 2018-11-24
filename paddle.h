#pragma once

#include <cstdint>
#include <d3d11.h>

#include "input.h"
#include "model.h"
#include "texture-shader.h"

class Paddle
{
public: // methods
	Paddle(float positionX);
	bool Initialize(ID3D11Device *pDevice);
	void Shutdown();
	void Update(uint64_t usdt, Input const &input);
	void Render(
		ID3D11DeviceContext *pDeviceContext, 
		TextureShader &shader,
		DirectX::XMMATRIX objectToWorld,
		DirectX::XMMATRIX worldToView,
		DirectX::XMMATRIX viewToClip);

private: // members
	float m_positionX;
	float m_positionY;
	float m_moveSpeedY;
	float m_height;

	Model m_model;
};
