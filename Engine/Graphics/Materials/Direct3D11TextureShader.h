#pragma once

#include <string>
#include <cstdint>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Direct3D11Shader.h"
#include "../Drivers/Direct3D11Renderer.h"
#include "../texture.h"
#include "../../utility.h"

class Direct3D11TextureShader : public Direct3D11Shader
{
private: // types
	struct MatrixBuffer
	{
		DirectX::XMMATRIX objectToWorld;
		DirectX::XMMATRIX worldToView;
		DirectX::XMMATRIX viewToClip;
	};

public: // methods
	Direct3D11TextureShader(
		Direct3D11Renderer &renderer,
		std::wstring textureFilename);

	virtual bool Initialize() override;
	virtual void Shutdown() override;
	virtual void Render(
		DirectX::XMMATRIX objectToWorld,
		DirectX::XMMATRIX worldToView,
		DirectX::XMMATRIX viewToClip) override;

private: // methods
	bool InitializeShader(
		ID3D11Device *pDevice,
		HWND hwnd,
		TCHAR *vsFilename,
		TCHAR *psFilename);
	bool InitializeInputLayout(
		ID3D11Device *pDevice,
		ID3D10Blob *pVertexShaderBuffer,
		ID3D11InputLayout **out_pInputLayout);
	void ShutdownShader();
	bool SetShaderParameters(
		ID3D11DeviceContext *pDeviceContext,
		DirectX::XMMATRIX objectToWorld,
		DirectX::XMMATRIX worldToView,
		DirectX::XMMATRIX viewToClip,
		ID3D11ShaderResourceView *pTexture);
	void RenderShader(ID3D11DeviceContext *pDeviceContext, uint32_t indexCount);

private: // members
	Direct3D11Renderer &m_renderer;
	std::wstring m_textureFilename;
	Texture m_texture;
	ScopedD3DPointer<ID3D11VertexShader> m_pVertexShader;
	ScopedD3DPointer<ID3D11PixelShader> m_pPixelShader;
	ScopedD3DPointer<ID3D11InputLayout> m_pInputLayout;
	ScopedD3DPointer<ID3D11Buffer> m_pMatrixBuffer;
	ScopedD3DPointer<ID3D11SamplerState> m_pSamplerState;
};