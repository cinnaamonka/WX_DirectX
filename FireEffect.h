#pragma once
#include "Texture.h"
#include "EffectBase.h"

namespace dae
{
	class FireEffect : public EffectBase
	{

	public:
		FireEffect() = default;
		FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~FireEffect();
	};
}


