#pragma once

#include <DirectXMath.h>

#include "d3d.h"
#include "../Engine/Core/input.h"
#include "../Engine/Graphics/model.h"
#include "paddle.h"
#include "../Engine/utility.h"
#include "../Engine/Graphics/Materials/texture-shader.h"

class Ball
{
public: // methods
	Ball();
	void Reset();
	bool Initialize(D3D &d3d);
	void Shutdown();
	void HandleEvents(PongEventList const &arrEvents);
	void Update(uint64_t usdt);
	void Render(
		D3D &d3d,
		TextureShader &shader,
		DirectX::XMMATRIX objectToWorld,
		DirectX::XMMATRIX worldToView,
		DirectX::XMMATRIX viewToClip);

	Vector2D GetVelocity() const;
	void SetVelocity(Vector2D const &velocity);
	Box2D const &GetBounds() const { return m_bounds; }

private: // members
	Box2D m_bounds;
	float m_velocityX;
	float m_velocityY;

	Model m_model;
};