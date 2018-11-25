#pragma once
#include <cstdint>

#include "d3d.h"
#include "camera.h"
#include "paddle.h"
#include "ball.h"
#include "input.h"
#include "color-shader.h"
#include "texture-shader.h"

class Scene
{
public: // methods
	Scene();
	bool Initialize(HWND hwnd, D3D &d3d);
	void Shutdown();
	void Reset();
	void Update(int64_t usDeltaTime, Input const &input);
	void Render(D3D &d3d);

private: // members
	TextureShader m_shader;
	Camera m_camera;
	Paddle m_playerPaddle;
	Paddle m_computerPaddle;
	Ball m_ball;
};
