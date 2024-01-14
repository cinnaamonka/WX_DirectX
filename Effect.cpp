#include "pch.h"
#include "Effect.h"
#include "Texture.h"
#include <fstream>
#include <cstring> 

namespace dae
{
	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile) :
		EffectBase(pDevice, assetFile, "DefaultTechnique")
	{
	}

	Effect::~Effect()
	{


	}
}