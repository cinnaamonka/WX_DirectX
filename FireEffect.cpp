#include "pch.h"
#include "FireEffect.h"

namespace dae
{
	FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile) :
		EffectBase(pDevice, assetFile,"FireEffectTechnique")
	{
		
	}

	FireEffect::~FireEffect()
	{


	}
}