#pragma once
#include <cstdint>

#include "d3d.h"
#include "camera.h"
#include "paddle.h"
#include "ball.h"
#include "input.h"
#include "color-shader.h"
#include "texture-shader.h"

const bool FULLSCREEN_ENABLED = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Scene
{
public: // methods
	Scene();
	bool Initialize(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd);
	void Shutdown();
	void Reset();
	void Update(int64_t usDeltaTime, Input const &input);
	void Render();

private: // members
	D3D m_d3d;
	TextureShader m_shader;
	Camera m_camera;
	Paddle m_playerPaddle;
	Paddle m_computerPaddle;
	Ball m_ball;
};
