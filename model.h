#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "texture.h"
#include "utility.h"

class Model
{
private: // types
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

public: // methods
	bool Initialize(ID3D11Device *pDevice);
	void Shutdown();
	void Render(ID3D11DeviceContext *pDeviceContext);
	uint32_t GetIndexCount() { return static_cast<uint32_t>(m_arrIndices.size()); }
	ID3D11ShaderResourceView *GetTexture() { return m_texture.GetTexture(); }

	Model(Model &&) = default;
	Model &operator=(Model &&) = default;
	Model(Model const &) = delete;
	Model& operator=(Model const &) = delete;

	static Model CreateQuad(float width, float height);

private: // methods
	Model();

	bool InitializeBuffers(ID3D11Device *pDevice);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext *pDeviceContext);

private: // members
	Texture m_texture;

	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pIndexBuffer;
	std::vector<VertexType> m_arrVertices;
	std::vector<uint32_t> m_arrIndices;
};
