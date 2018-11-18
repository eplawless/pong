#include "color-shader.h"
#include "utility.h"

#include <d3dcompiler.h>
#include <fstream>

ColorShader::ColorShader()
	: m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pInputLayout(nullptr)
	, m_pMatrixBuffer(nullptr)
{
}

bool ColorShader::Initialize(
	ID3D11Device *pDevice, 
	HWND hwnd)
{
	TCHAR *vsFilename = TEXT("../Pong/color.vs");
	TCHAR *psFilename = TEXT("../Pong/color.ps");
	return InitializeShader(pDevice, hwnd, vsFilename, psFilename);
}

void ColorShader::Shutdown()
{
	ShutdownShader();
}

bool ColorShader::Render(
	ID3D11DeviceContext *pDeviceContext, 
	uint32_t indexCount, 
	DirectX::XMMATRIX worldMatrix, 
	DirectX::XMMATRIX viewMatrix, 
	DirectX::XMMATRIX projectionMatrix)
{
	if (!SetShaderParameters(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	RenderShader(pDeviceContext, indexCount);
	return true;
}

bool ColorShader::InitializeShader(
	ID3D11Device *pDevice, 
	HWND hwnd, 
	TCHAR *vsFilename,
	TCHAR *psFilename)
{
	ID3D10Blob* pErrorMessage = nullptr;
	AtExit cleanUpErrorMessage{ [&]() { if (pErrorMessage) pErrorMessage->Release(); } };

	ID3D10Blob* pVertexShaderBuffer = nullptr;
	HRESULT vsCompileOk = D3DCompileFromFile(
		vsFilename,
		nullptr,
		nullptr,
		"ColorVertexShader",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pVertexShaderBuffer,
		&pErrorMessage
	);
	if (FAILED(vsCompileOk))
	{
		pErrorMessage
			? OutputShaderErrorMessage(pErrorMessage, hwnd, vsFilename)
			: MessageBox(hwnd, vsFilename, TEXT("Missing Shader File"), MB_OK);
		return false;
	}
	AtExit cleanUpVertexShaderBuffer{ [&]() { pVertexShaderBuffer->Release(); } };

	ID3D10Blob* pPixelShaderBuffer;
	HRESULT psCompileOk = D3DCompileFromFile(
		psFilename,
		nullptr,
		nullptr,
		"ColorPixelShader",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pPixelShaderBuffer,
		&pErrorMessage
	);
	if (FAILED(psCompileOk))
	{
		pErrorMessage
			? OutputShaderErrorMessage(pErrorMessage, hwnd, psFilename)
			: MessageBox(hwnd, psFilename, TEXT("Missing Shader File"), MB_OK);
		return false;
	}
	AtExit cleanUpPixelShaderBuffer{ [&]() { pPixelShaderBuffer->Release(); } };

	HRESULT vsCreateOk = pDevice->CreateVertexShader(
		pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(),
		nullptr,
		&m_pVertexShader
	);
	if (FAILED(vsCreateOk))
	{
		return false;
	}

	HRESULT psCreateOk = pDevice->CreatePixelShader(
		pPixelShaderBuffer->GetBufferPointer(),
		pPixelShaderBuffer->GetBufferSize(),
		nullptr,
		&m_pPixelShader
	);
	if (FAILED(psCreateOk))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC arrVertexInputLayout[2];
	arrVertexInputLayout[0].SemanticName = "POSITION";
	arrVertexInputLayout[0].SemanticIndex = 0;
	arrVertexInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	arrVertexInputLayout[0].InputSlot = 0;
	arrVertexInputLayout[0].AlignedByteOffset = 0;
	arrVertexInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrVertexInputLayout[0].InstanceDataStepRate = 0;

	arrVertexInputLayout[1].SemanticName = "COLOR";
	arrVertexInputLayout[1].SemanticIndex = 0;
	arrVertexInputLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	arrVertexInputLayout[1].InputSlot = 0;
	arrVertexInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	arrVertexInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrVertexInputLayout[1].InstanceDataStepRate = 0;

	uint32_t numElements = sizeof(arrVertexInputLayout) / sizeof(*arrVertexInputLayout);
	HRESULT createInputLayoutOk = pDevice->CreateInputLayout(
		arrVertexInputLayout,
		numElements,
		pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(),
		&m_pInputLayout
	);
	if (FAILED(createInputLayoutOk))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	if (FAILED(pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer)))
	{
		return false;
	}

	return true;
}

void ColorShader::ShutdownShader()
{
	if (m_pMatrixBuffer)
	{
		m_pMatrixBuffer->Release();
		m_pMatrixBuffer = nullptr;
	}

	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}

	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
		m_pPixelShader = nullptr;
	}

	if (m_pVertexShader)
	{
		m_pVertexShader->Release();
		m_pVertexShader = nullptr;
	}
}

void ColorShader::OutputShaderErrorMessage(
	ID3D10Blob *pErrorMessage, 
	HWND hwnd, 
	TCHAR *shaderFilename)
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

bool ColorShader::SetShaderParameters(
	ID3D11DeviceContext *pDeviceContext, 
	DirectX::XMMATRIX worldMatrix, 
	DirectX::XMMATRIX viewMatrix, 
	DirectX::XMMATRIX projectionMatrix)
{
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT lockBufferOk = pDeviceContext->Map(
		m_pMatrixBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);
	if (FAILED(lockBufferOk))
	{
		return false;
	}

	MatrixBufferType* pConstantBufferData = static_cast<MatrixBufferType*>(mappedResource.pData);
	pConstantBufferData->world = worldMatrix;
	pConstantBufferData->view = viewMatrix;
	pConstantBufferData->projection = projectionMatrix;

	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	uint32_t bufferNumber = 0;
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer);

	return true;
}

void ColorShader::RenderShader(
	ID3D11DeviceContext *pDeviceContext, 
	uint32_t indexCount)
{
	pDeviceContext->IASetInputLayout(m_pInputLayout);
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}
