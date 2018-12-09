#pragma once

#include <DirectXMath.h>

#include "../Engine/Core/Window/Window.h"
#include "../Engine/Graphics/Drivers/Renderer.h"
#include "../Engine/Graphics/Materials/Shader.h"
#include "../Engine/Graphics/model.h"
#include "../Engine/utility.h"
#include "paddle.h"

class Ball
{
public: // methods
	Ball();
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

	Vector2D GetVelocity() const;
	void SetVelocity(Vector2D const &velocity);
	Box2D const &GetBounds() const { return m_bounds; }

private: // members
	Box2D m_bounds;
	float m_velocityX;
	float m_velocityY;

	Model m_model;
};