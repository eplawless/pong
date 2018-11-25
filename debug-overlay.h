#pragma once

#include "d3d.h"

class DebugOverlay
{
public: // methods
	DebugOverlay();
 	bool Initialize(HWND hwnd, D3D &d3d);
 	void Shutdown();
	bool IsVisible() const { return m_isVisible; }
	void SetVisible(bool isVisible);
	void Render();
	LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private: // members
	bool m_isVisible;
	bool m_option;
};