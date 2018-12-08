#include "color-shader.h"
#include "../../utility.h"

#include <d3dcompiler.h>
#include <fstream>

bool ColorShader::Initialize(
	ID3D11Device *pDevice, 
	HWND hwnd)
{
	WCHAR *vsFilename = L"Engine/Graphics/Materials/color-vs.hlsl";
	WCHAR *psFilename = L"Engine/Graphics/Materials/color-ps.hlsl";
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
	WCHAR *vsFilename,
	WCHAR *psFilename)
{
	auto onVertexShaderInitialized = [&](ID3D10Blob *pVertexShaderBuffer) -> bool 
	{
		return InitializeVertexLayout(pDevice, pVertexShaderBuffer, &m_pInputLayout.Get());
	};

	bool vsOk = InitializeVertexShader(pDevice, hwnd, vsFilename, "ColorVertexShader", &m_pVertexShader.Get(), onVertexShaderInitialized);
	if (!vsOk)
	{
		return false;
	}

	bool psOk = InitializePixelShader(pDevice, hwnd, psFilename, "ColorPixelShader", &m_pPixelShader.Get());
	if (!psOk)
	{
		return false;
	}

	bool cbOk = InitializeConstantBuffer<MatrixBufferType>(pDevice, &m_pMatrixBuffer.Get());
	if (!cbOk)
	{
		return false;
	}

	return true;
}

bool ColorShader::InitializeVertexLayout(
	ID3D11Device *pDevice,
	ID3D10Blob *pVertexShaderBuffer,
	ID3D11InputLayout **out_pInputLayout)
{
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
		out_pInputLayout
	);
	if (FAILED(createInputLayoutOk))
	{
		return false;
	}

	return true;
}

void ColorShader::ShutdownShader()
{
	m_pMatrixBuffer.Reset();
	m_pInputLayout.Reset();
	m_pPixelShader.Reset();
	m_pVertexShader.Reset();
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
		m_pMatrixBuffer.Get(),
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

	pDeviceContext->Unmap(m_pMatrixBuffer.Get(), 0);

	uint32_t bufferNumber = 0;
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer.Get());

	return true;
}

void ColorShader::RenderShader(
	ID3D11DeviceContext *pDeviceContext, 
	uint32_t indexCount)
{
	pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
	pDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}
