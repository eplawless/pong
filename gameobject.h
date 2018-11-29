#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include "d3d.h"
#include "pong-event.h"
#include "camera.h"

class GameObject
{
public: // methods
	virtual ~GameObject() {}
	bool Initialize(D3D &d3d);
	void Shutdown();
	virtual void Reset() = 0;
	virtual void HandleEvents(GameEventList const &arrGameEvents) = 0;
	virtual void Update(uint64_t usdt) = 0;
	virtual void Render() = 0;

protected: // methods
	virtual bool InitializeImpl() = 0;
	virtual void ShutdownImpl() = 0;

protected: // members
	D3D *m_pD3D;
};
