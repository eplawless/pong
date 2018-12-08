#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include "../Graphics/Drivers/d3d.h"

class GameObject
{
public: // methods
	virtual ~GameObject() {}
	bool Initialize(D3D &d3d);
	void Shutdown();
	virtual void Reset() {};
	virtual void Update(uint64_t usdt) {};
	virtual void Render() {};

protected: // methods
	virtual bool InitializeImpl() {};
	virtual void ShutdownImpl() {};

protected: // members
	D3D *m_pD3D;
};
