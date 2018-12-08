#pragma once

#include <functional>
#include <cstdint>
#include <d3d11.h>
#include <DirectXMath.h>

class Shader
{
public: // methods
	Shader();
	virtual ~Shader();

protected: // methods
	void OutputShaderErrorMessage(
		ID3D10Blob *pErrorMessage,
		HWND hwnd,
		WCHAR *shaderFilename);

	bool InitializeVertexShader(
		ID3D11Device *pDevice,
		HWND hwnd,
		WCHAR *filename,
		CHAR *entryPoint,
		ID3D11VertexShader **out_pVertexShader,
		std::function<bool(ID3D10Blob*)> postProcessAction);

	bool InitializePixelShader(
		ID3D11Device *pDevice,
		HWND hwnd,
		WCHAR *filename,
		CHAR *entryPoint,
		ID3D11PixelShader **out_pPixelShader);

	template <typename BufferType>
	bool InitializeConstantBuffer(
		ID3D11Device *pDevice,
		ID3D11Buffer **out_pBuffer)
	{
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(BufferType);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		if (FAILED(pDevice->CreateBuffer(&bufferDesc, nullptr, out_pBuffer)))
		{
			return false;
		}

		return true;
	}
};