#pragma once
#include <string>
#include <SDL.h>
#include <d3d11.h>
#include "Vector3.h"  // Include appropriate headers for Vector3 and Vector2
#include "Vector2.h"
#include "ColorRGB.h"
namespace dae
{
	struct Vector2;

	class Texture
	{
	public:
		Texture(SDL_Surface* pSurface, ID3D11Device* device);
		~Texture();
		static Texture* LoadFromFile(const std::string& path, ID3D11Device* device);
		/*ColorRGB Sample(const Vector2& uv) const;
		Vector3 SampleNormalMap(const Vector2& uv) const;*/
		ID3D11ShaderResourceView* GetShaderResourceView() const;
		class ReadEmptytexture : public std::exception
		{
		public:
			 virtual const char* what() const throw()
			{
				return "Texture not loaded";
			}
		};
	private:
		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };
		ID3D11ShaderResourceView* m_pShaderResourceView;
		ID3D11Texture2D* m_pTexture = nullptr;
	};
}