#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include "../Graphics/Drivers/Direct3D11Renderer.h"

class GameObject
{
public: // methods
	virtual ~GameObject() {}
	bool Initialize(Direct3D11Renderer &d3d);
	void Shutdown();
	virtual void Reset() {};
	virtual void Update(uint64_t usdt) {};
	virtual void Render() {};

protected: // methods
	virtual bool InitializeImpl() {};
	virtual void ShutdownImpl() {};

protected: // members
	Direct3D11Renderer *m_pD3D;
};
