#include "d3d.h"

#include <vector>
#include <cassert>

#include "utility.h"

static const uint32_t BYTES_PER_MEGABYTE = 1024 * 1024;

D3D::D3D()
	: m_vsyncEnabled(false)
	, m_isFullscreen(false)
	, m_videoCardMemoryInMB(0)
	, m_pSwapChain(nullptr)
	, m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
	, m_pRenderTargetView(nullptr)
	, m_pDepthStencilBuffer(nullptr)
	, m_pDepthStencilState(nullptr)
	, m_pDepthStencilView(nullptr)
	, m_pRasterizerState(nullptr)
	, m_projectionMatrix(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
	, m_worldMatrix(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
	, m_orthoMatrix(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
{
	ZeroMemory(&m_videoCardDescription, sizeof(m_videoCardDescription) / sizeof(*m_videoCardDescription));
}

bool D3D::Initialize(
	uint32_t windowWidth, 
	uint32_t windowHeight, 
	bool vsyncEnabled, 
	HWND hwnd, 
	bool isFullscreen, 
	float screenDepth, 
	float screenNear)
{
	m_vsyncEnabled = vsyncEnabled;
	m_isFullscreen = isFullscreen;

	uint32_t refreshRateNumerator = 0;
	uint32_t refreshRateDenominator = 1;
	bool deviceInfoOk = InitializeDeviceInfo(
		windowWidth, 
		windowHeight,
		&refreshRateNumerator,
		&refreshRateDenominator,
		&m_videoCardMemoryInMB,
		m_videoCardDescription
	);
	if (!deviceInfoOk)
	{
		return false;
	}

	bool swapChainOk = InitializeSwapChainAndDevice(
		hwnd,
		windowWidth, 
		windowHeight, 
		refreshRateNumerator, 
		refreshRateDenominator,
		&m_pSwapChain,
		&m_pDevice,
		&m_pDeviceContext
	);
	if (!swapChainOk)
	{
		return false;
	}

	bool renderTargetViewOk = InitializeRenderTargetView(&m_pRenderTargetView);
	if (!renderTargetViewOk)
	{
		return false;
	}

	bool depthStencilBufferOk = InitializeDepthStencil(
		windowWidth,
		windowHeight,
		&m_pDepthStencilBuffer,
		&m_pDepthStencilState,
		&m_pDepthStencilView
	);
	if (!depthStencilBufferOk)
	{
		return false;
	}

	bool rasterizerOk = InitializeRasterizerState(&m_pRasterizerState);
	if (!rasterizerOk)
	{
		return false;
	}

	bool viewportOk = InitializeViewport(windowWidth, windowHeight);
	if (!viewportOk)
	{
		return false;
	}

	bool matricesOk = InitializeMatrices(
		windowWidth,
		windowHeight,
		screenNear,
		screenDepth,
		&m_projectionMatrix,
		&m_worldMatrix,
		&m_orthoMatrix
	);
	if (!matricesOk)
	{
		return false;
	}

	return true;
}

void D3D::Shutdown()
{
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(false, nullptr);
	}

	if (m_pRasterizerState)
	{
		m_pRasterizerState->Release();
		m_pRasterizerState = nullptr;
	}

	if (m_pDepthStencilView)
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;
	}

	if (m_pDepthStencilState)
	{
		m_pDepthStencilState->Release();
		m_pDepthStencilState = nullptr;
	}

	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = nullptr;
	}

	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;
	}

	if (m_pDeviceContext)
	{
		m_pDeviceContext->Release();
		m_pDeviceContext = nullptr;
	}

	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = nullptr;
	}
}

void D3D::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4] = { red, green, blue, alpha };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::EndScene()
{
	m_pSwapChain->Present(m_vsyncEnabled ? 1 : 0, 0);
}

void D3D::GetVideoCardInfo(
	char *out_videoCardName, 
	size_t maxVideoCardNameLength, 
	int32_t &out_videoCardMemoryInMB)
{
	size_t copyLimit = DirectX::XMMin(VIDEO_CARD_DESCRIPTION_MAX_LENGTH, maxVideoCardNameLength);
	strcpy_s(out_videoCardName, copyLimit, m_videoCardDescription);
	out_videoCardMemoryInMB = m_videoCardMemoryInMB;
}

void D3D::GetProjectionMatrix(DirectX::XMMATRIX &out_projectionMatrix)
{
	out_projectionMatrix = XMLoadFloat4x4(&m_projectionMatrix);
}

void D3D::GetWorldMatrix(DirectX::XMMATRIX &out_worldMatrix)
{
	out_worldMatrix = XMLoadFloat4x4(&m_worldMatrix);
}

void D3D::GetOrthoMatrix(DirectX::XMMATRIX &out_orthoMatrix)
{
	out_orthoMatrix = XMLoadFloat4x4(&m_orthoMatrix);
}

bool D3D::InitializeDeviceInfo(
	uint32_t windowWidth, 
	uint32_t windowHeight,
	uint32_t *out_refreshRateNumerator,
	uint32_t *out_refreshRateDenominator,
	uint32_t *out_videoCardMemoryInMB,
	char *out_videoCardDescription)
{
	IDXGIFactory *pFactory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
	{
		return false;
	}

	AtExit cleanUpFactory{ [&]() { pFactory->Release(); } };

	IDXGIAdapter *pAdapter;
	if (FAILED(pFactory->EnumAdapters(0, &pAdapter)))
	{
		return false;
	}
	AtExit cleanUpAdapter{ [&]() { pAdapter->Release(); } };

	IDXGIOutput *pAdapterOutput;
	if (FAILED(pAdapter->EnumOutputs(0, &pAdapterOutput)))
	{
		return false;
	}
	AtExit cleanUpAdapterOutput{ [&]() { pAdapterOutput->Release(); } };

	uint32_t numDisplayModes = 0;
	HRESULT gotNumDisplayModes = pAdapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr);
	if (FAILED(gotNumDisplayModes))
	{
		return false;
	}

	std::vector<DXGI_MODE_DESC> arrDisplayModes;
	arrDisplayModes.resize(numDisplayModes);
	HRESULT gotDisplayModeList = pAdapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, 
		DXGI_ENUM_MODES_INTERLACED, 
		&numDisplayModes, 
		arrDisplayModes.data()
	);
	if (FAILED(gotDisplayModeList))
	{
		return false;
	}

	bool foundMatchingDisplayMode = false;
	for (uint32_t idx = 0; idx < numDisplayModes; ++idx)
	{
		const DXGI_MODE_DESC &displayMode = arrDisplayModes[idx];
		if (displayMode.Width == windowWidth && displayMode.Height == windowHeight)
		{
			foundMatchingDisplayMode = true;
			*out_refreshRateNumerator = displayMode.RefreshRate.Numerator;
			*out_refreshRateDenominator = displayMode.RefreshRate.Denominator;
			break;
		}
	}
	if (!foundMatchingDisplayMode)
	{
		return false;
	}

	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(pAdapter->GetDesc(&adapterDesc)))
	{
		return false;
	}

	*out_videoCardMemoryInMB = static_cast<uint32_t>(adapterDesc.DedicatedVideoMemory / BYTES_PER_MEGABYTE);
	size_t videoCardNameLength = 0;
	int32_t nameConversionError = wcstombs_s(
		&videoCardNameLength,
		out_videoCardDescription,
		VIDEO_CARD_DESCRIPTION_MAX_LENGTH,
		adapterDesc.Description,
		VIDEO_CARD_DESCRIPTION_MAX_LENGTH
	);
	if (nameConversionError != 0)
	{
		return false;
	}

	return true;
}

bool D3D::InitializeSwapChainAndDevice(
	HWND hwnd,
	uint32_t windowWidth, 
	uint32_t windowHeight, 
	uint32_t refreshRateNumerator, 
	uint32_t refreshRateDenominator,
	IDXGISwapChain **out_pSwapChain, 
	ID3D11Device **out_pDevice, 
	ID3D11DeviceContext **out_pDeviceContext)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = m_vsyncEnabled ? refreshRateNumerator : 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = m_vsyncEnabled ? refreshRateDenominator : 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !m_isFullscreen;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;

	HRESULT didCreateDeviceAndSwapChain = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		out_pSwapChain,
		out_pDevice,
		nullptr,
		out_pDeviceContext
	);
	if (FAILED(didCreateDeviceAndSwapChain))
	{
		return false;
	}

	return true;
}

bool D3D::InitializeRenderTargetView(
	ID3D11RenderTargetView **out_pRenderTargetView)
{
	assert(m_pSwapChain != nullptr);
	assert(m_pDevice != nullptr);

	ID3D11Texture2D* pBackBuffer;
	HRESULT gotBuffer = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBackBuffer);
	if (FAILED(gotBuffer))
	{
		return false;
	}
	AtExit cleanUpBackBuffer{ [&]() { pBackBuffer->Release(); } };

	HRESULT result = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, out_pRenderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool D3D::InitializeDepthStencil(
	uint32_t windowWidth,
	uint32_t windowHeight,
	ID3D11Texture2D **out_pDepthStencilBuffer, 
	ID3D11DepthStencilState **out_pDepthStencilState, 
	ID3D11DepthStencilView **out_pDepthStencilView)
{
	assert(m_pDevice != nullptr);
	assert(m_pDeviceContext != nullptr);
	assert(m_pRenderTargetView != nullptr);

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthBufferDesc.Width = windowWidth;
	depthBufferDesc.Height = windowHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	HRESULT bufferOk = m_pDevice->CreateTexture2D(&depthBufferDesc, nullptr, out_pDepthStencilBuffer);
	if (FAILED(bufferOk))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HRESULT stateOk = m_pDevice->CreateDepthStencilState(&depthStencilDesc, out_pDepthStencilState);
	if (FAILED(stateOk))
	{
		return false;
	}

	m_pDeviceContext->OMSetDepthStencilState(*out_pDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HRESULT viewOk = m_pDevice->CreateDepthStencilView(
		*out_pDepthStencilBuffer,
		&depthStencilViewDesc,
		out_pDepthStencilView
	);
	if (FAILED(viewOk))
	{
		return false;
	}

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, *out_pDepthStencilView);

	return true;
}

bool D3D::InitializeRasterizerState(
	ID3D11RasterizerState **out_pRasterizerState)
{
	assert(m_pDevice != nullptr);
	assert(m_pDeviceContext != nullptr);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	HRESULT rasterizerOk = m_pDevice->CreateRasterizerState(&rasterizerDesc, out_pRasterizerState);
	if (FAILED(rasterizerOk))
	{
		return false;
	}

	m_pDeviceContext->RSSetState(*out_pRasterizerState);

	return true;
}

bool D3D::InitializeViewport(
	uint32_t windowWidth, 
	uint32_t windowHeight)
{
	assert(m_pDeviceContext != nullptr);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(viewport));
	viewport.Width = static_cast<float>(windowWidth);
	viewport.Height = static_cast<float>(windowHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_pDeviceContext->RSSetViewports(1, &viewport);

	return true;
}

bool D3D::InitializeMatrices(
	uint32_t windowWidth, 
	uint32_t windowHeight, 
	float screenNear,
	float screenDepth,
	DirectX::XMFLOAT4X4 *out_projectionMatrix, 
	DirectX::XMFLOAT4X4 *out_worldMatrix, 
	DirectX::XMFLOAT4X4 *out_orthoMatrix)
{
	float windowWidthF = static_cast<float>(windowWidth);
	float windowHeightF = static_cast<float>(windowHeight);
	float fieldOfView = DirectX::XM_PIDIV4;
	float aspectRatio = windowWidthF / windowHeightF;
	DirectX::XMStoreFloat4x4(out_projectionMatrix, 
		DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, screenNear, screenDepth));
	DirectX::XMStoreFloat4x4(out_worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(out_orthoMatrix,
		DirectX::XMMatrixOrthographicLH(windowWidthF, windowHeightF, screenNear, screenDepth));

	return true;
}
