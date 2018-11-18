#pragma once

#include <cstdint>
#include <d3d11.h>
#include <DirectXMath.h>

class ColorShader
{
private: // types
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

public: // methods
	ColorShader();

	bool Initialize(ID3D11Device *pDevice, HWND hwnd);
	void Shutdown();
	bool Render(
		ID3D11DeviceContext *pDeviceContext, 
		uint32_t indexCount,
		DirectX::XMMATRIX worldMatrix,
		DirectX::XMMATRIX viewMatrix,
		DirectX::XMMATRIX projectionMatrix);

private: // methods
	bool InitializeShader(
		ID3D11Device *pDevice,
		HWND hwnd,
		TCHAR *vsFilename,
		TCHAR *psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(
		ID3D10Blob *pErrorMessage, 
		HWND hwnd, 
		TCHAR *shaderFilename);
	bool SetShaderParameters(
		ID3D11DeviceContext *pDeviceContext,
		DirectX::XMMATRIX worldMatrix,
		DirectX::XMMATRIX viewMatrix,
		DirectX::XMMATRIX projectionMatrix);
	void RenderShader(ID3D11DeviceContext *pDeviceContext, uint32_t indexCount);

private: // members
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pPixelShader;
	ID3D11InputLayout *m_pInputLayout;
	ID3D11Buffer *m_pMatrixBuffer;
};