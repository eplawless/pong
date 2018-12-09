#include "Direct3D11Renderer.h"

#include <vector>
#include <cassert>

#include "../../utility.h"
#include "../Materials/Direct3D11TextureShader.h"

static const uint32_t BYTES_PER_MEGABYTE = 1024 * 1024;

Direct3D11Renderer::Direct3D11Renderer(Win32Window &window)
	: m_window(window)
	, m_vsyncEnabled(false)
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
{
	ZeroMemory(&m_videoCardDescription, sizeof(m_videoCardDescription) / sizeof(*m_videoCardDescription));
}

bool Direct3D11Renderer::Initialize(
	bool vsyncEnabled, 
	bool isFullscreen, 
	float screenDepth, 
	float screenNear)
{
	m_vsyncEnabled = vsyncEnabled;
	m_isFullscreen = isFullscreen;

	HRESULT wicOk = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(wicOk))
	{
		return false;
	}

	uint32_t windowWidth = m_window.GetWidth();
	uint32_t windowHeight = m_window.GetHeight();
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
		m_window.GetHandle(),
		windowWidth, 
		windowHeight, 
		refreshRateNumerator, 
		refreshRateDenominator,
		&m_pSwapChain.Get(),
		&m_pDevice.Get(),
		&m_pDeviceContext.Get()
	);
	if (!swapChainOk)
	{
		return false;
	}

	bool renderTargetViewOk = InitializeRenderTargetView(&m_pRenderTargetView.Get());
	if (!renderTargetViewOk)
	{
		return false;
	}

	bool depthStencilBufferOk = InitializeDepthStencil(
		windowWidth,
		windowHeight,
		&m_pDepthStencilBuffer.Get(),
		&m_pDepthStencilState.Get(),
		&m_pDepthStencilView.Get()
	);
	if (!depthStencilBufferOk)
	{
		return false;
	}

	bool rasterizerOk = InitializeRasterizerState(&m_pRasterizerState.Get());
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

void Direct3D11Renderer::Shutdown()
{
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(false, nullptr);
	}
	m_pRasterizerState.Reset();
	m_pDepthStencilView.Reset();
	m_pDepthStencilState.Reset();
	m_pDepthStencilBuffer.Reset();
	m_pRenderTargetView.Reset();
	m_pDeviceContext.Reset();
	m_pDevice.Reset();
	m_pSwapChain.Reset();
}

void Direct3D11Renderer::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4] = { red, green, blue, alpha };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Direct3D11Renderer::EndScene()
{
	m_pSwapChain->Present(m_vsyncEnabled ? 1 : 0, 0);
}

Shader::SharedPointer Direct3D11Renderer::GetTextureShader(std::wstring const &textureFilename)
{
	return Shader::SharedPointer(new Direct3D11TextureShader(*this, textureFilename));
}

void Direct3D11Renderer::GetVideoCardInfo(
	char *out_videoCardName, 
	size_t maxVideoCardNameLength, 
	int32_t &out_videoCardMemoryInMB)
{
	size_t copyLimit = DirectX::XMMin(VIDEO_CARD_DESCRIPTION_MAX_LENGTH, maxVideoCardNameLength);
	strcpy_s(out_videoCardName, copyLimit, m_videoCardDescription);
	out_videoCardMemoryInMB = m_videoCardMemoryInMB;
}

DirectX::XMMATRIX Direct3D11Renderer::GetProjectionMatrix()
{
	return XMLoadFloat4x4(&m_projectionMatrix);
}

DirectX::XMMATRIX Direct3D11Renderer::GetWorldMatrix()
{
	return XMLoadFloat4x4(&m_worldMatrix);
}

DirectX::XMMATRIX Direct3D11Renderer::GetOrthoMatrix()
{
	return XMLoadFloat4x4(&m_orthoMatrix);
}

bool Direct3D11Renderer::InitializeDeviceInfo(
	uint32_t windowWidth, 
	uint32_t windowHeight,
	uint32_t *out_refreshRateNumerator,
	uint32_t *out_refreshRateDenominator,
	uint32_t *out_videoCardMemoryInMB,
	char *out_videoCardDescription)
{
	ScopedD3DPointer<IDXGIFactory> pFactory;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory.Get())))
	{
		return false;
	}

	ScopedD3DPointer<IDXGIAdapter> pAdapter;
	if (FAILED(pFactory->EnumAdapters(0, &pAdapter.Get())))
	{
		return false;
	}

	ScopedD3DPointer<IDXGIOutput> pAdapterOutput;
	if (FAILED(pAdapter->EnumOutputs(0, &pAdapterOutput.Get())))
	{
		return false;
	}

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

bool Direct3D11Renderer::InitializeSwapChainAndDevice(
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

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT deviceAndSwapChainOk = D3D11CreateDeviceAndSwapChain(
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
	if (FAILED(deviceAndSwapChainOk))
	{
		return false;
	}

	return true;
}

bool Direct3D11Renderer::InitializeRenderTargetView(
	ID3D11RenderTargetView **out_pRenderTargetView)
{
	assert(m_pSwapChain);
	assert(m_pDevice);

	ScopedD3DPointer<ID3D11Texture2D> pBackBuffer;
	HRESULT gotBuffer = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBackBuffer.Get());
	if (FAILED(gotBuffer))
	{
		return false;
	}

	HRESULT result = m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, out_pRenderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Direct3D11Renderer::InitializeDepthStencil(
	uint32_t windowWidth,
	uint32_t windowHeight,
	ID3D11Texture2D **out_pDepthStencilBuffer, 
	ID3D11DepthStencilState **out_pDepthStencilState, 
	ID3D11DepthStencilView **out_pDepthStencilView)
{
	assert(m_pDevice);
	assert(m_pDeviceContext);
	assert(m_pRenderTargetView);

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

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView.Get(), *out_pDepthStencilView);

	return true;
}

bool Direct3D11Renderer::InitializeRasterizerState(
	ID3D11RasterizerState **out_pRasterizerState)
{
	assert(m_pDevice);
	assert(m_pDeviceContext);

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

bool Direct3D11Renderer::InitializeViewport(
	uint32_t windowWidth, 
	uint32_t windowHeight)
{
	assert(m_pDeviceContext);

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

bool Direct3D11Renderer::InitializeMatrices(
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
