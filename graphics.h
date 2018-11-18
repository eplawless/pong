#pragma once
#include <cstdint>

#include "d3d.h"

const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public: // methods
	void Initialize(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd);
	void Shutdown();
	void Update(int64_t usDeltaTime);

private: // members
	D3D m_d3d;
};
