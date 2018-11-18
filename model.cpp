#include "model.h"

#include <cstdint>
#include <vector>

Model::Model()
	: m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
	, m_vertexCount(0)
	, m_indexCount(0)
{
}

bool Model::Initialize(
	ID3D11Device *pDevice)
{
	return InitializeBuffers(pDevice);
}

void Model::Shutdown()
{
	ShutdownBuffers();
}

void Model::Render(
	ID3D11DeviceContext *pDeviceContext)
{
	RenderBuffers(pDeviceContext);
}

bool Model::InitializeBuffers(
	ID3D11Device *pDevice)
{
	std::vector<VertexType> arrVertices;
	std::vector<uint32_t> arrIndices;
	DirectX::XMFLOAT4 green{ 0.0f, 1.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 red{ 1.0f, 0.0f, 0.0f, 1.0f };
	CreateTriangle(red, &m_vertexCount, &m_indexCount, &arrVertices, &arrIndices);

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = arrVertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer)))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(size_t) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = arrIndices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer)))
	{
		return false;
	}

	return true;
}

void Model::ShutdownBuffers()
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
	}

	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
}

void Model::RenderBuffers(
	ID3D11DeviceContext *pDeviceContext)
{
	uint32_t vertexStride = sizeof(VertexType);
	uint32_t vertexOffset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &vertexStride, &vertexOffset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::CreateTriangle(
	const DirectX::XMFLOAT4 &color,
	uint32_t *out_vertexCount,
	uint32_t *out_indexCount,
	std::vector<VertexType> *out_arrVertices, 
	std::vector<uint32_t> *out_arrIndices)
{
	*out_vertexCount = 3;
	*out_indexCount = 3;
	
	out_arrVertices->resize(*out_vertexCount);
	out_arrIndices->resize(*out_indexCount);

	(*out_arrVertices)[0].position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f); // bottom left
	(*out_arrVertices)[0].color = color;
	(*out_arrVertices)[1].position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f); // top middle
	(*out_arrVertices)[1].color = color;
	(*out_arrVertices)[2].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f); // bottom right
	(*out_arrVertices)[2].color = color;

	(*out_arrIndices)[0] = 0; // bottom left
	(*out_arrIndices)[1] = 1; // top middle
	(*out_arrIndices)[2] = 2; // bottom right
}
