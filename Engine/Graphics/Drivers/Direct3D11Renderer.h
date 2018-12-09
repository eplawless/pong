#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTK.lib")

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "../../utility.h"
#include "../../Core/Window/Win32Window.h"

#include "Renderer.h"
#include "../Materials/shader.h"

class Direct3D11Renderer : public Renderer
{
public: // methods
	Direct3D11Renderer(Win32Window &window);

	// TODO: implement fullscreen
	virtual bool Initialize(bool vsyncEnabled, bool isFullscreen, float screenDepth, float screenNear) override;
	virtual void Shutdown() override;
	virtual void BeginScene(float red, float green, float blue, float alpha) override;
	virtual void EndScene() override;

	virtual Shader::SharedPointer GetTextureShader(std::wstring const &textureFilename) override;

	virtual DirectX::XMMATRIX GetProjectionMatrix() override;
	virtual DirectX::XMMATRIX GetWorldMatrix() override;
	virtual DirectX::XMMATRIX GetOrthoMatrix() override;

	HWND GetWindowHandle() { return m_window.GetHandle(); }

	ID3D11Device *GetDevice() { return m_pDevice.Get(); }
	ID3D11DeviceContext *GetDeviceContext() { return m_pDeviceContext.Get(); }

	void GetVideoCardInfo(
		char *out_videoCardName, 
		size_t maxVideoCardNameLength, 
		int32_t &out_videoCardMemoryInMB);

private: // methods
	bool InitializeDeviceInfo(
		uint32_t windowWidth, 
		uint32_t windowHeight,
		uint32_t *out_refreshRateNumerator,
		uint32_t *out_refreshRateDenominator,
		uint32_t *out_videoCardMemoryInMB,
		char *out_videoCardDescription);

	bool InitializeSwapChainAndDevice(
		HWND hwnd,
		uint32_t windowWidth,
		uint32_t windowHeight,
		uint32_t refreshRateNumerator,
		uint32_t refreshRateDenominator,
		IDXGISwapChain **out_pSwapChain,
		ID3D11Device **out_pDevice,
		ID3D11DeviceContext **out_pDeviceContext);

	bool InitializeRenderTargetView(
		ID3D11RenderTargetView **out_pRenderTargetView);

	bool InitializeDepthStencil(
		uint32_t windowWidth,
		uint32_t windowHeight,
		ID3D11Texture2D **out_pDepthStencilBuffer,
		ID3D11DepthStencilState **out_pDepthStencilState,
		ID3D11DepthStencilView **out_pDepthStencilView);

	bool InitializeRasterizerState(
		ID3D11RasterizerState **out_pRasterizerState);

	bool InitializeViewport(
		uint32_t windowWidth, 
		uint32_t windowHeight);

	bool InitializeMatrices(
		uint32_t windowWidth,
		uint32_t windowHeight,
		float screenNear,
		float screenDepth,
		DirectX::XMFLOAT4X4 *out_projectionMatrix,
		DirectX::XMFLOAT4X4 *out_worldMatrix,
		DirectX::XMFLOAT4X4 *out_orthoMatrix
	);

private: // static members
	static const size_t VIDEO_CARD_DESCRIPTION_MAX_LENGTH = 128;

private: // members
	Win32Window &m_window;
	bool m_vsyncEnabled;
	bool m_isFullscreen;
	uint32_t m_videoCardMemoryInMB;
	char m_videoCardDescription[VIDEO_CARD_DESCRIPTION_MAX_LENGTH];
	ScopedD3DPointer<IDXGISwapChain> m_pSwapChain;
	ScopedD3DPointer<ID3D11Device> m_pDevice;
	ScopedD3DPointer<ID3D11DeviceContext> m_pDeviceContext;
	ScopedD3DPointer<ID3D11RenderTargetView> m_pRenderTargetView;
	ScopedD3DPointer<ID3D11Texture2D> m_pDepthStencilBuffer;
	ScopedD3DPointer<ID3D11DepthStencilState> m_pDepthStencilState;
	ScopedD3DPointer<ID3D11DepthStencilView> m_pDepthStencilView;
	ScopedD3DPointer<ID3D11RasterizerState> m_pRasterizerState;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_orthoMatrix;
};