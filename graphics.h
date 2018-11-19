#pragma once
#include <cstdint>

#include "d3d.h"
#include "camera.h"
#include "model.h"
#include "color-shader.h"
#include "texture-shader.h"

const bool FULLSCREEN_ENABLED = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public: // methods
	bool Initialize(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd);
	void Shutdown();
	void Update(int64_t usDeltaTime);

private: // members
	D3D m_d3d;
	Camera m_camera;
	Model m_model;
	//ColorShader m_shader;
	TextureShader m_shader;
};
