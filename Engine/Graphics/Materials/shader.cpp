#include "shader.h"
#include "../../utility.h"

#include <fstream>
#include <d3dcompiler.h>
#include <cstdlib>

Shader::Shader()
{
}

Shader::~Shader()
{
}

void Shader::OutputShaderErrorMessage(
	ID3D10Blob *pErrorMessage, 
	HWND hwnd, 
	WCHAR *shaderFilename)
{
	const char *compileErrors = static_cast<const char *>(pErrorMessage->GetBufferPointer());
	size_t bufferSize = pErrorMessage->GetBufferSize();

	std::ofstream fout;
	fout.open("shader-error.log");
	fout.write(compileErrors, bufferSize);
	fout.close();
	
	MessageBox(
		hwnd,
		TEXT("Error compiling shader. Check shader-error.log for details."),
		shaderFilename,
		MB_OK
	);
}

bool Shader::InitializeVertexShader(
	ID3D11Device *pDevice,
	HWND hwnd,
	WCHAR *filename,
	CHAR *entryPoint,
	ID3D11VertexShader **out_pVertexShader,
	std::function<bool(ID3D10Blob*)> postProcessAction)
{
	ID3D10Blob* pErrorMessage = nullptr;
	AtExit cleanUpErrorMessage{ [&]() { if (pErrorMessage) pErrorMessage->Release(); } };

	ID3D10Blob* pVertexShaderBuffer = nullptr;
	HRESULT vsCompileOk = D3DCompileFromFile(
		filename,
		nullptr,
		nullptr,
		entryPoint,
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pVertexShaderBuffer,
		&pErrorMessage
	);
	if (FAILED(vsCompileOk))
	{
		pErrorMessage
			? OutputShaderErrorMessage(pErrorMessage, hwnd, filename)
			: MessageBox(hwnd, filename, TEXT("Missing Shader File"), MB_OK);
		return false;
	}
	AtExit cleanUpVertexShaderBuffer{ [&]() { pVertexShaderBuffer->Release(); } };

	HRESULT vsCreateOk = pDevice->CreateVertexShader(
		pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(),
		nullptr,
		out_pVertexShader
	);
	if (FAILED(vsCreateOk))
	{
		return false;
	}

	if (postProcessAction && !postProcessAction(pVertexShaderBuffer))
	{
		return false;
	}

	return true;
}

bool Shader::InitializePixelShader(
	ID3D11Device *pDevice, 
	HWND hwnd, 
	WCHAR *filename, 
	CHAR *entryPoint, 
	ID3D11PixelShader **out_pPixelShader)
{
	ID3D10Blob* pErrorMessage = nullptr;
	AtExit cleanUpErrorMessage{ [&]() { if (pErrorMessage) pErrorMessage->Release(); } };

	ID3D10Blob* pPixelShaderBuffer;
	HRESULT psCompileOk = D3DCompileFromFile(
		filename,
		nullptr,
		nullptr,
		entryPoint,
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pPixelShaderBuffer,
		&pErrorMessage
	);
	if (FAILED(psCompileOk))
	{
		pErrorMessage
			? OutputShaderErrorMessage(pErrorMessage, hwnd, filename)
			: MessageBox(hwnd, filename, TEXT("Missing Shader File"), MB_OK);
		return false;
	}
	AtExit cleanUpPixelShaderBuffer{ [&]() { pPixelShaderBuffer->Release(); } };

	HRESULT psCreateOk = pDevice->CreatePixelShader(
		pPixelShaderBuffer->GetBufferPointer(),
		pPixelShaderBuffer->GetBufferSize(),
		nullptr,
		out_pPixelShader
	);
	if (FAILED(psCreateOk))
	{
		return false;
	}

	return true;
}
