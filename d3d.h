#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#include <dxgi.h>
#include <d3d11.h>
#include <DirectXMath.h>

class D3D
{
public: // methods
	D3D();

	bool Initialize(
		uint32_t windowWidth, 
		uint32_t windowHeight,
		bool vsyncEnabled, 
		HWND hwnd, 
		bool isFullscreen, // TODO: this
		float screenDepth,
		float screenNear);

	void Shutdown();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device *GetDevice() { return m_pDevice; }
	ID3D11DeviceContext *GetDeviceContext() { return m_pDeviceContext; }

	void GetVideoCardInfo(
		char *out_videoCardName, 
		size_t maxVideoCardNameLength, 
		int32_t &out_videoCardMemoryInMB);

	void GetProjectionMatrix(DirectX::XMMATRIX &out_projectionMatrix);
	void GetWorldMatrix(DirectX::XMMATRIX &out_worldMatrix);
	void GetOrthoMatrix(DirectX::XMMATRIX &out_orthoMatrix);

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
	bool m_vsyncEnabled;
	bool m_isFullscreen;
	uint32_t m_videoCardMemoryInMB;
	char m_videoCardDescription[VIDEO_CARD_DESCRIPTION_MAX_LENGTH];
	IDXGISwapChain *m_pSwapChain;
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pDeviceContext;
	ID3D11RenderTargetView *m_pRenderTargetView;
	ID3D11Texture2D *m_pDepthStencilBuffer;
	ID3D11DepthStencilState *m_pDepthStencilState;
	ID3D11DepthStencilView *m_pDepthStencilView;
	ID3D11RasterizerState *m_pRasterizerState;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_orthoMatrix;
};