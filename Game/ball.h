#pragma once

#include <DirectXMath.h>

#include "../Engine/Core/Window/Window.h"
#include "../Engine/Graphics/Drivers/Renderer.h"
#include "../Engine/Graphics/Materials/Shader.h"
#include "../Engine/Graphics/model.h"
#include "../Engine/Physics/Physics.h"
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
	Box2D GetBounds() const;

private: // members
	Physics::ObjectHandle m_hCircle;
	Model m_model;
};