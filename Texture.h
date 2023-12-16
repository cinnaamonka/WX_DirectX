#pragma once
#include <string>
#include <SDL.h>
#include <d3d11.h>
#include "Vector3.h" 
#include "Vector2.h"
#include "ColorRGB.h"
#include "pch.h"

namespace dae
{
	struct Vector2;

	class Texture
	{
	public:
		Texture();
		~Texture();
		static void LoadFromFile(const std::string& path, ID3D11Device* device);
		inline ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; };
		class ReadEmptytexture : public std::exception
		{
		public:
			virtual const char* what() const throw()
			{
				return "Texture not loaded";
			}
		};
	private:
		static ID3D11ShaderResourceView* m_pShaderResourceView;
	};
}