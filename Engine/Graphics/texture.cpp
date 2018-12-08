#include "texture.h"

#include "../Libraries/DirectXTK/Include/WICTextureLoader.h"

bool Texture::Initialize(
	ID3D11Device *pDevice, 
	WCHAR *filename)
{
	HRESULT textureOk = DirectX::CreateWICTextureFromFile(
		pDevice,
		filename,
		&m_pTexture.Get(),
		&m_pShaderResourceView.Get()
	);
	if (FAILED(textureOk))
	{
		return false;
	}

	return true;
}

void Texture::Shutdown()
{
	m_pShaderResourceView.Reset();
	m_pTexture.Reset();
}
