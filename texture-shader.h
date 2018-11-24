#pragma once

#include "shader.h"
#include "texture.h"
#include "utility.h"

#include <cstdint>
#include <d3d11.h>
#include <DirectXMath.h>

class TextureShader : Shader
{
private: // types
	struct MatrixBuffer
	{
		DirectX::XMMATRIX objectToWorld;
		DirectX::XMMATRIX worldToView;
		DirectX::XMMATRIX viewToClip;
	};

public: // methods
	bool Initialize(ID3D11Device *pDevice, HWND hwnd);
	void Shutdown();
	bool Render(
		ID3D11DeviceContext *pDeviceContext,
		uint32_t indexCount,
		DirectX::XMMATRIX objectToWorld,
		DirectX::XMMATRIX worldToView,
		DirectX::XMMATRIX viewToClip,
		ID3D11ShaderResourceView *pTexture);

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
	ScopedD3DPointer<ID3D11VertexShader> m_pVertexShader;
	ScopedD3DPointer<ID3D11PixelShader> m_pPixelShader;
	ScopedD3DPointer<ID3D11InputLayout> m_pInputLayout;
	ScopedD3DPointer<ID3D11Buffer> m_pMatrixBuffer;
	ScopedD3DPointer<ID3D11SamplerState> m_pSamplerState;
};