#include "Texture.h"
#include "Vector3.h"
#include "Vector2.h"
#include <SDL_image.h>

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface, ID3D11Device* device):
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels },
		m_pShaderResourceView(nullptr)
	{	
		// Create a D3D11 texture description based on surface information

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = pSurface->w; 
		textureDesc.Height = pSurface->h; 
		textureDesc.MipLevels = 1; 
		textureDesc.ArraySize = 1; 
		textureDesc.Format = format;
		  // Adjust the format based on your needs
		textureDesc.SampleDesc.Count = 1; 
		textureDesc.SampleDesc.Quality = 0; 
		textureDesc.Usage = D3D11_USAGE_DEFAULT; 
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
		textureDesc.CPUAccessFlags = 0; 
		textureDesc.MiscFlags = 0; 

		// Create a D3D11 texture using the SDL_Surface data
		D3D11_SUBRESOURCE_DATA initData = {}; 
		initData.pSysMem = pSurface->pixels; 
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch); 
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		
		HRESULT hr = device->CreateTexture2D(&textureDesc, &initData, &m_pTexture);
		
		if (SUCCEEDED(hr))
		{
			// Create a shader resource view
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			hr = device->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pShaderResourceView);

			// Release the temporary texture as it is no longer needed
			m_pTexture->Release();
		}

		// Release SDL_Surface memory
		SDL_FreeSurface(pSurface);
	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path, ID3D11Device* device)
	{
		//TODO
		
		 SDL_Surface* pSurface = IMG_Load(path.c_str());
		
		 Texture* textureObject = new Texture(pSurface,device);
		//Load SDL_Surface using IMG_LOAD
		//Create & Return a new Texture Object (using SDL_Surface)
		

		return textureObject;
	}
	ID3D11ShaderResourceView* Texture::GetShaderResourceView() const
	{
		return m_pShaderResourceView;
	}
}