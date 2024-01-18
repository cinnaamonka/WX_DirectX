// ReSharper disable CppFunctionResultShouldBeUsed
#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Utils.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow, Camera* pCamera) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
		m_pCamera = new Camera(pCamera->cameraOrigin, pCamera->fovAngle);
		m_pCamera->Initialize(static_cast<float>(m_Width) / static_cast<float>(m_Height), m_ShouldSpeedBeIncreased, 45.f, Vector3{ 0.0f, 0.0f ,-50.0f });
		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";

			std::vector<Vertex>   vehicle_vertices{};
			std::vector<uint32_t> vehicle_indices{};
			std::vector<Vertex>   fireFx_vertices{};
			std::vector<uint32_t> fireFx_indices{};

			if (!Utils::ParseOBJ("Resources/vehicle.obj", vehicle_vertices, vehicle_indices))
			{
				std::cout << "Object initialization failed!\n";
			}

			if (!Utils::ParseOBJ("Resources/fireFX.obj", fireFx_vertices, fireFx_indices))
			{
				std::cout << "Object initialization failed!\n";
			}
			m_pMesh = new Mesh(m_pDevice, vehicle_vertices, vehicle_indices, L"./Resources/PosCol3D.fx", "DefaultTechnique");
			m_pFireFXMesh = new Mesh(m_pDevice, fireFx_vertices, fireFx_indices, L"./Resources/FireShader.fx", "FireEffectTechnique");

			// loading everything for fire mesh
			m_pFireTexture = std::make_unique<Texture>(); 
			m_pFireTexture->LoadFromFile("Resources/fireFX_diffuse.png", m_pDevice);
			m_pFireFXMesh->SetDiffuseMap(m_pFireTexture.get());

			m_pDiffuseTexture = new Texture();
			m_pDiffuseTexture->LoadFromFile("Resources/vehicle_diffuse.png", m_pDevice);
			m_pMesh->SetDiffuseMap(m_pDiffuseTexture);

			// Load and set specular map
			m_pSpecularTexture = new Texture();
			m_pSpecularTexture->LoadFromFile("Resources/vehicle_specular.png", m_pDevice);
			m_pMesh->SetSpecularMap(m_pSpecularTexture);

			// Load and set glossiness map
			m_pGlossinessTexture = new Texture();
			m_pGlossinessTexture->LoadFromFile("Resources/vehicle_gloss.png", m_pDevice);
			m_pMesh->SetGlossinessMap(m_pGlossinessTexture);

			// Load and set normal map
			m_pNormalTexture = new Texture();
			m_pNormalTexture->LoadFromFile("Resources/vehicle_normal.png", m_pDevice);
			m_pMesh->SetNormalMap(m_pNormalTexture);

		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

	}
	Renderer::~Renderer()
	{

		if (m_pRenderTargetView) m_pRenderTargetView->Release();
		if (m_pDepthStencilView) m_pDepthStencilView->Release();

		if (m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();
		if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();

		if (m_pSwapChain) m_pSwapChain->Release();

		if (m_pDeviceContext) 
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		if (m_pDevice) m_pDevice->Release();
		if (m_pCamera)
		{
			delete m_pCamera;
			m_pCamera = nullptr;
		}
		if (m_pNormalTexture)
		{
			delete m_pNormalTexture;
			m_pNormalTexture = nullptr;
		}
		if (m_pDiffuseTexture)
		{
			delete m_pDiffuseTexture;
			m_pDiffuseTexture = nullptr;
		}
		if (m_pSpecularTexture)
		{
			delete m_pSpecularTexture;
			m_pSpecularTexture = nullptr;
		}
		if (m_pGlossinessTexture)
		{
			delete m_pGlossinessTexture;
			m_pGlossinessTexture = nullptr;
		}
		
		if (m_pMesh)
		{
			delete m_pMesh;
			m_pMesh = nullptr;
		}
		if (m_pFireFXMesh)
		{
			delete m_pFireFXMesh;
			m_pFireFXMesh = nullptr;
		}
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);
		m_pMesh->SetCameraOrigin(m_pCamera->GetOrigin());

		if (m_CanBeRotated)
		{
			const float angle = PI_DIV_4 * pTimer->GetTotal();

			m_pMesh->RotateY(angle);
			m_pFireFXMesh->RotateY(angle);

		}
		
		m_pCamera->ChangeMovementSpeed(m_ShouldSpeedBeIncreased);
	}

	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		// 1. CLEAR RTV & DSV
		constexpr float color[4] = { 0.39f, 0.59f, 0.93f, 1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		// 2. SET PIPELINE + INVOKE DRAW CALLS (= RENDER)
		const auto worldMatrix = m_pMesh->GetWorldMatrix();

		auto viewProjectionMatrix = worldMatrix * m_pCamera->worldViewProjectionMatrix;

		m_pMesh->Render(m_pDeviceContext, &viewProjectionMatrix, &worldMatrix);

		if (m_FireMeshShouldBeShown)
		{
			m_pFireFXMesh->Render(m_pDeviceContext, &viewProjectionMatrix, &worldMatrix);
		}
		
		//// 3. PRESENT BACKBUFFER (SWAP)
		m_pSwapChain->Present(0, 0);
	}

	std::string Renderer::ConvertSamplerStateToString(SamplerState state)
	{
		{
			switch (state)
			{
			case SamplerState::Point:
				return "Point";
			case SamplerState::Linear:
				return "Linear";
			case SamplerState::Anisotropic:
				return "Anisotropic";
			default:
				return "Unknown";
			}
		}
	}

	HRESULT Renderer::InitializeDirectX()
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
			0, createDeviceFlags, &featureLevel, 1,
			D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
		if (FAILED(result))
			return result;

		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		// Get the handle (HWND) from from the SDL backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (pDxgiFactory) pDxgiFactory->Release();

		if (FAILED(result))
			return result;

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

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		return S_OK;
	}

	void Renderer::CycleSamplerState()
	{
		m_SampleState = static_cast<SamplerState>((static_cast<int>(m_SampleState) + 1) % 3);
		m_pMesh->SetSampleState(static_cast<UINT>(m_SampleState));

		SamplerState state = m_SampleState;
		std::string stateStr = ConvertSamplerStateToString(state);

		std::cout << "Current texture sampling state is: " << stateStr << std::endl;
	}
}
