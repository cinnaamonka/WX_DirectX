#include "pch.h"
#include "Effect.h"
#include "Texture.h"
#include <fstream>

namespace dae
{
	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile) :
		EffectBase(pDevice, assetFile)
	{

	}

	Effect::~Effect()
	{


	}
}