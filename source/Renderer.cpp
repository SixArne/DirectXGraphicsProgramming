#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		std::vector<Vertex> vertices{
			{{-3.f, 3.f, -2.f}		,{1.f, 1.f, 1.f},	{0.0f, 0.0f}},
			{{0.f, 3.f, -2.f}		,{1.f, 1.f, 1.f},	{0.5f, 0.0f}},
			{{3.f, 3.0f, -2.f}		,{1.f, 1.f, 1.f},	{1.0f, 0.f}},
			{{-3.f, 0.f, -2.f}		,{1.f, 1.f, 1.f},	{0.f, 0.5f}},
			{{0.f, 0.f, -2.f}		,{1.f, 1.f, 1.f},	{0.5f, 0.5f}},
			{{3.f, 0.f, -2.f}		,{1.f, 1.f, 1.f},	{1.0f, 0.5f}},
			{{-3.f, -3.f, -2.f}		,{1.f, 1.f, 1.f},	{0.0f, 1.0f}},
			{{0.f, -3.f, -2.f}		,{1.f, 1.f, 1.f},	{0.5f, 1.0f}},
			{{3.f, -3.f, -2.f}		,{1.f, 1.f, 1.f},	{1.0f, 1.0f}},
		};

		std::vector<uint32_t> indices{
			3,0,1,  1,4,3,  4,1,2,
			2,5,4,  6,3,4,  4,7,6,
			7,4,5,  5,8,7,
		};

		m_pDiffuseTexture = Texture::LoadFromFile(m_pDevice, "Resources/uv_grid_2.png");

		m_pMesh = new Mesh(m_pDevice, vertices, indices);
		m_pMesh->SetTexture(m_pDiffuseTexture);

		m_pCamera = new Camera();
		m_pCamera->Initialize((float)m_Width / (float)m_Height, 45.f, { 0,0, -10.f });
	}

	Renderer::~Renderer()
	{
		if (m_pDeviceContext)
		{
			m_pRenderTargetView->Release();
			m_pRenderTargetBuffer->Release();
			m_pDepthStencilView->Release();
			m_pDepthStencilBuffer->Release();
			m_pSwapChain->Release();

			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();

			m_pDevice->Release();

			delete m_pMesh;
		}

		delete m_pCamera;

		if (m_pDiffuseTexture != nullptr)
		{
			delete m_pDiffuseTexture;
		}
	}

	void Renderer::Update(Timer* pTimer)
	{
		m_pCamera->Update(pTimer);
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		// Clear RTV and DSV
		ColorRGB clearColor = ColorRGB{ 0.f, 0.f, 0.3f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Setup pipeline + invoke draw calls
		Matrix wvp = m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix();
		m_pMesh->Render(m_pDeviceContext, wvp);

		// Present back buffer
		m_pSwapChain->Present(0, 0);
	}

	HRESULT Renderer::InitializeDirectX()
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Create device to interface with D11
		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
		if (FAILED(result))
		{
			throw std::runtime_error("Error creating device");
			return result;
		}

		// Setup swapchain
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
		{
			throw std::runtime_error("Error creating factory");
			return result;
		}

		// Create swapchain info struct
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		// Get handle (HWND) from the SDL Back buffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
		{
			throw std::runtime_error("Error creating swapchain");
			return result;
		}

		// Setup depth buffer
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		// Setup depth buffer view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
		{
			throw std::runtime_error("Error creating texture for stencil");
			return result;
		}

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
		{
			throw std::runtime_error("Error creating view for stencil");
			return result;
		}

		// Create render target and render target view
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
		{
			throw std::runtime_error("Error getting backbuffer");
			return result;
		}

		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
		{
			throw std::runtime_error("Error creating render target view");
			return result;
		}

		// Bind RTV and DSV to output merger stage
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.0f;

		m_pDeviceContext->RSSetViewports(1, &viewport);

		m_IsInitialized = true;

		// Clean up factory after use
		pDxgiFactory->Release();

		return result;
	}
}
