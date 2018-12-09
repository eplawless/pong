#pragma once

#include <memory>
#include <DirectXMath.h>

class Shader
{
public: // types
	using SharedPointer = std::shared_ptr<Shader>;

public: // methods
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void Render(
		DirectX::XMMATRIX objectToWorld,
		DirectX::XMMATRIX worldToView,
		DirectX::XMMATRIX viewToClip) = 0;

protected: // methods
	Shader() = default;
	virtual ~Shader() = default;
};