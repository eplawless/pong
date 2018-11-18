#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

class Model
{
private: // types
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

public: // methods
	Model();
	bool Initialize(ID3D11Device *pDevice);
	void Shutdown();
	void Render(ID3D11DeviceContext *pDeviceContext);
	uint32_t GetIndexCount() { return m_indexCount; }

private: // methods
	bool InitializeBuffers(ID3D11Device *pDevice);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext *pDeviceContext);

	void CreateTriangle(
		const DirectX::XMFLOAT4 &color,
		uint32_t *out_vertexCount,
		uint32_t *out_indexCount,
		std::vector<VertexType> *out_arrVertices,
		std::vector<uint32_t> *out_arrIndices);

private: // members
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pIndexBuffer;
	uint32_t m_vertexCount;
	uint32_t m_indexCount;
};
