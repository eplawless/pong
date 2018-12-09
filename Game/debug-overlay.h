#pragma once

#include "../Engine/Graphics/Drivers/Direct3D11Renderer.h"

class DebugOverlay
{
public: // types
	struct Options
	{
		bool isPaused = false;
	};

public: // methods
	DebugOverlay();
 	bool Initialize(HWND hwnd, Direct3D11Renderer &d3d);
 	void Shutdown();
	bool IsVisible() const { return m_isVisible; }
	void SetVisible(bool isVisible);
	Options const &GetOptions() const { return m_options; }
	void Render();
	LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private: // members
	bool m_isVisible;
	Options m_options;
};