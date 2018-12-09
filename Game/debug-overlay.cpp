#include "debug-overlay.h"
#include "../Engine/Libraries/DearIMGUI/imgui.h"
#include "../Engine/Libraries/DearIMGUI/imgui_impl_win32.h"
#include "../Engine/Libraries/DearIMGUI/imgui_impl_dx11.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DebugOverlay::DebugOverlay()
	: m_isVisible(false)
{
}

bool DebugOverlay::Initialize(
	HWND hwnd, 
	Direct3D11Renderer &d3d)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(d3d.GetDevice(), d3d.GetDeviceContext());
	ImGui::StyleColorsClassic();
	return true;
}

void DebugOverlay::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DebugOverlay::SetVisible(bool isVisible)
{
	m_isVisible = isVisible;
}

void DebugOverlay::Render()
{
	if (!m_isVisible) { return; }

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug Menu");
	if (ImGui::Button(m_options.isPaused ? "Unpause" : "Pause"))
	{
		m_options.isPaused = !m_options.isPaused;
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

LRESULT DebugOverlay::HandleMessage(
	HWND hwnd, 
	UINT message, 
	WPARAM wparam, 
	LPARAM lparam)
{
	return ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam);
}
