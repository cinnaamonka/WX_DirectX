#pragma once
#include "Texture.h"
#include "EffectBase.h"

namespace dae
{
	class Effect : public EffectBase
	{
	public:
		Effect() = default;
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~Effect();
	};

}
