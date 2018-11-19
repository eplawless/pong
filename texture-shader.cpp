#include "texture-shader.h"

bool TextureShader::Initialize(
	ID3D11Device *pDevice, 
	HWND hwnd)
{
	TCHAR *vsFilename = TEXT("../Pong/texture-vs.hlsl");
	TCHAR *psFilename = TEXT("../Pong/texture-ps.hlsl");
	if (!InitializeShader(pDevice, hwnd, vsFilename, psFilename))
	{
		return false;
	}

	return true;
}

void TextureShader::Shutdown()
{
	ShutdownShader();
}

bool TextureShader::Render(
	ID3D11DeviceContext *pDeviceContext, 
	uint32_t indexCount, 
	DirectX::XMMATRIX worldMatrix, 
	DirectX::XMMATRIX viewMatrix, 
	DirectX::XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView *pTexture)
{
	if (!SetShaderParameters(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix, pTexture))
	{
		return false;
	}

	RenderShader(pDeviceContext, indexCount);
	return true;
}

bool TextureShader::InitializeShader(
	ID3D11Device *pDevice, 
	HWND hwnd, 
	TCHAR *vsFilename, 
	TCHAR *psFilename)
{
	auto onVertexShaderInitialized = [&](ID3D10Blob *pVertexShaderBuffer) -> bool
	{
		return InitializeVertexLayout(pDevice, pVertexShaderBuffer, &m_pInputLayout.Get());
	};

	bool vsOk = InitializeVertexShader(pDevice, hwnd, vsFilename, "TextureVertexShader", &m_pVertexShader.Get(), onVertexShaderInitialized);
	if (!vsOk)
	{
		return false;
	}

	bool psOk = InitializePixelShader(pDevice, hwnd, psFilename, "TexturePixelShader", &m_pPixelShader.Get());
	if (!psOk)
	{
		return false;
	}

	bool cbOk = InitializeConstantBuffer<MatrixBufferType>(pDevice, &m_pMatrixBuffer.Get());
	if (!cbOk)
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT samplerStateOk = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState.Get());
	if (FAILED(samplerStateOk))
	{
		return false;
	}

	return true;
}

bool TextureShader::InitializeVertexLayout(
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

	arrVertexInputLayout[1].SemanticName = "TEXCOORD";
	arrVertexInputLayout[1].SemanticIndex = 0;
	arrVertexInputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
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

void TextureShader::ShutdownShader()
{
	m_pSamplerState.Reset();
	m_pMatrixBuffer.Reset();
	m_pInputLayout.Reset();
	m_pPixelShader.Reset();
	m_pVertexShader.Reset();
}

bool TextureShader::SetShaderParameters(
	ID3D11DeviceContext *pDeviceContext, 
	DirectX::XMMATRIX worldMatrix, 
	DirectX::XMMATRIX viewMatrix, 
	DirectX::XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView *pTexture)
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
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	return true;
}

void TextureShader::RenderShader(
	ID3D11DeviceContext *pDeviceContext, 
	uint32_t indexCount)
{
	pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
	pDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState.Get());
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}
