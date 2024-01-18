#pragma once
#include "pch.h"
#include "Texture.h"
namespace dae
{
	ID3D11ShaderResourceView* Texture::m_pShaderResourceView = nullptr;

	Texture::Texture()
	{

	}

	Texture::~Texture()
	{
		if (m_pShaderResourceView)
		{
			Texture::m_pShaderResourceView->Release();
			Texture::m_pShaderResourceView = nullptr;
		}
	}

	void Texture::LoadFromFile(const std::string& path, ID3D11Device* device)
	{
		SDL_Surface* pSurface = IMG_Load(path.c_str());

		if (pSurface == nullptr)
		{
			printf("Error loading image: %s\n", IMG_GetError());
			return;
		}

		ID3D11Texture2D* textureObject = nullptr;
		HRESULT hr = S_OK;

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = pSurface->w;
		textureDesc.Height = pSurface->h;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		hr = device->CreateTexture2D(&textureDesc, &initData, &textureObject);

		if (FAILED(hr))
		{
			SDL_FreeSurface(pSurface);
			if (textureObject) textureObject->Release();
			return;
		}

		if (m_pShaderResourceView) m_pShaderResourceView->Release();

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(textureObject, &srvDesc, &m_pShaderResourceView);

		if (textureObject) textureObject->Release();
		SDL_FreeSurface(pSurface);

		if (FAILED(hr))
		{
			if (m_pShaderResourceView) m_pShaderResourceView->Release();
			return;
		}

	}

}


