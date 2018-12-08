#include "gameobject.h"

bool GameObject::Initialize(D3D &d3d)
{
	m_pD3D = &d3d;
	return InitializeImpl();
}

void GameObject::Shutdown()
{
	ShutdownImpl();
	m_pD3D = nullptr;
}
