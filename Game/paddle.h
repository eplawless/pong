#pragma once

#include <cstdint>

#include "pong-event.h"
#include "../Engine/Core/Window/Window.h"
#include "../Engine/Graphics/Drivers/Renderer.h"
#include "../Engine/Graphics/Materials/Shader.h"
#include "../Engine/Graphics/model.h"
#include "../Engine/utility.h"

class Paddle
{
public: // types
	enum class Side { Left, Right };
	enum Movement { Up = 1, Down = -1, Stopped = 0 };

public: // methods
	Paddle(float positionX, Side side);
	void Reset();
	bool Initialize(Window &window, Renderer &renderer);
	void Shutdown();
	void HandleEvents(PongEventList const &arrEvents);
	void Update(uint64_t usdt);
	void Render(
		Renderer &renderer, 
		Shader &shader,
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
