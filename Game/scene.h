#pragma once
#include <cstdint>

#include "../Engine/Core/Window/Window.h"
#include "../Engine/Graphics/Drivers/Renderer.h"
#include "../Engine/Graphics/Materials/Shader.h"
#include "../Engine/Graphics/camera.h"

#include "paddle.h"
#include "ball.h"
#include "pong-event.h"

class Scene
{
public: // methods
	Scene();
	bool Initialize(Window &window, Renderer &renderer);
	void Shutdown();
	void Reset();
	void HandleEvents(PongEventList const &arrEvents);
	void Update(int64_t usDeltaTime);
	void Render(Renderer &renderer);

private: // methods
	bool CollideBallWithPaddles();
	bool CollideBallWithGoals();

private: // members
	Shader::SharedPointer m_textureShader;
	Camera m_camera;
	Paddle m_leftPaddle;
	Paddle m_rightPaddle;
	Ball m_ball;
};
