#pragma once

struct SDL_Window;
struct SDL_Surface;

class Mesh;

#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#ifdef _DEBUG
#include "vld.h"
#endif // _DEBUG

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow, Camera* pCamera);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void CycleSamplerState();

		void RotateModel()
		{
			m_CanBeRotated = !m_CanBeRotated;
		}

		void IncreaseCameraSpeed()
		{
			m_ShouldSpeedBeIncreased = !m_ShouldSpeedBeIncreased;
		}

		void ToggleNormalMap()
		{
			m_ModelShouldBeRotated = !m_ModelShouldBeRotated;
			m_pMesh->ToggleNormalMap(m_ModelShouldBeRotated);
		}

		void ToggleFireMeshVisibility()
		{
			m_FireMeshShouldBeShown = !m_FireMeshShouldBeShown;
		}

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		enum class SamplerState
		{
			Point,
			Linear,
			Anisotropic,
			Count
		};

		SamplerState m_SampleState = SamplerState::Point;

		std::string ConvertSamplerStateToString(SamplerState state);
		
		bool m_IsInitialized = false;
		bool m_CanBeRotated = false;

		//DIRECTX
		HRESULT InitializeDirectX();
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Texture2D* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		Mesh* m_pMesh = nullptr; 
		Mesh* m_pFireFXMesh = nullptr; 
		Camera* m_pCamera;

		Texture* m_pDiffuseTexture;
		Texture* m_pSpecularTexture;
		Texture* m_pGlossinessTexture;
		Texture* m_pNormalTexture;
		std::unique_ptr<Texture> m_pFireTexture;

		bool m_ShouldSpeedBeIncreased = false;
		bool m_ModelShouldBeRotated = false;
		bool m_FireMeshShouldBeShown = true;
	};
}
