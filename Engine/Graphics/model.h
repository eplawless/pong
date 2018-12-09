#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "../utility.h"
#include "../Core/Window/Window.h"
#include "Drivers/Renderer.h"

class Model
{
private: // types
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

public: // methods
	bool Initialize(Window &window, Renderer &renderer);
	void Shutdown();
	void Render(Renderer &renderer);
	uint32_t GetIndexCount() { return static_cast<uint32_t>(m_arrIndices.size()); }

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
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pIndexBuffer;
	std::vector<VertexType> m_arrVertices;
	std::vector<uint32_t> m_arrIndices;
};
