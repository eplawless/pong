#pragma once
#include <cstdint>

#include "d3d.h"
#include "camera.h"
#include "paddle.h"
#include "ball.h"
#include "pong-event.h"
#include "color-shader.h"
#include "texture-shader.h"

class Scene
{
public: // methods
	Scene();
	bool Initialize(D3D &d3d);
	void Shutdown();
	void Reset();
	void HandleEvents(PongEventList const &arrEvents);
	void Update(int64_t usDeltaTime);
	void Render(D3D &d3d);

private: // methods
	bool CollideBallWithPaddles();
	bool CollideBallWithGoals();

private: // members
	TextureShader m_shader;
	Camera m_camera;
	Paddle m_leftPaddle;
	Paddle m_rightPaddle;
	Ball m_ball;
};
