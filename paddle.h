#pragma once

#include <cstdint>

#include "d3d.h"
#include "pong-event.h"
#include "model.h"
#include "utility.h"
#include "texture-shader.h"
#include "gameobject.h"

class Paddle
{
public: // types
	enum class Side { Left, Right };
	enum Movement { Up = 1, Down = -1, Stopped = 0 };

public: // methods
	Paddle(float positionX, Side side);
	void Reset();
	bool Initialize(D3D &d3d);
	void Shutdown();
	void HandleEvents(GameEventList const &arrEvents);
	void Update(uint64_t usdt);
	void Render(
		D3D &d3d, 
		TextureShader &shader,
		DirectX::XMMATRIX objectToWorld,
		DirectX::XMMATRIX worldToView,
		DirectX::XMMATRIX viewToClip);

	Box2D const &GetBounds() const { return m_bounds; }

private: // members
	Side m_side;
	Box2D m_bounds;
	Movement m_moveState;
	float m_moveSpeedY;
	float m_height;

	Model m_model;
};
