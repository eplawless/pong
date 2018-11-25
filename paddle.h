#pragma once

#include <cstdint>
#include <d3d11.h>

#include "input.h"
#include "model.h"
#include "utility.h"
#include "texture-shader.h"

class Paddle
{
public: // methods
	Paddle(float positionX);
	void Reset();
	bool Initialize(ID3D11Device *pDevice);
	void Shutdown();
	void Update(uint64_t usdt, Input const &input);
	void Render(
		ID3D11DeviceContext *pDeviceContext, 
		TextureShader &shader,
		DirectX::XMMATRIX objectToWorld,
		DirectX::XMMATRIX worldToView,
		DirectX::XMMATRIX viewToClip);

	Box2D const &GetBounds() const { return m_bounds; }

private: // members
	Box2D m_bounds;
	float m_moveSpeedY;
	float m_height;

	Model m_model;
};
