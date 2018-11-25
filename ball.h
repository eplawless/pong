#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "input.h"
#include "model.h"
#include "paddle.h"
#include "utility.h"
#include "texture-shader.h"

class Ball
{
public: // types
	enum class UpdateResult
	{
		Moved,
		HitWall,
		HitPaddle,
		HitLeftGoal,
		HitRightGoal
	};

public: // methods
	Ball();
	void Reset();
	bool Initialize(ID3D11Device *pDevice);
	void Shutdown();
	UpdateResult Update(uint64_t usdt, Paddle const &leftPaddle, Paddle const &rightPaddle);
	void Render(
		ID3D11DeviceContext *pDeviceContext,
		TextureShader &shader,
		DirectX::XMMATRIX objectToWorld,
		DirectX::XMMATRIX worldToView,
		DirectX::XMMATRIX viewToClip);

	Box2D const &GetBounds() const { return m_bounds; }

private: // members
	Box2D m_bounds;
	float m_velocityX;
	float m_velocityY;

	Model m_model;
};