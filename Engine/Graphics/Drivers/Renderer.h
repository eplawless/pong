#pragma once

#include <memory>
#include <string>
#include <DirectXMath.h>

#include "../Materials/shader.h"

class Renderer
{
public: // methods
	virtual bool Initialize(bool vsyncEnabled, bool isFullscreen, float screenDepth, float screenNear) = 0;
	virtual void Shutdown() = 0;
	virtual void BeginScene(float red, float green, float blue, float alpha) = 0;
	virtual void EndScene() = 0;

	virtual Shader::SharedPointer GetTextureShader(std::wstring const &textureFilename) = 0;

	virtual DirectX::XMMATRIX GetProjectionMatrix() = 0;
	virtual DirectX::XMMATRIX GetWorldMatrix() = 0;
	virtual DirectX::XMMATRIX GetOrthoMatrix() = 0;

protected: // methods
	Renderer() = default;
	virtual ~Renderer() = default;
};
