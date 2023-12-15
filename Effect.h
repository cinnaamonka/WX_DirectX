#pragma once
#include "Texture.h"

namespace dae
{
	class Effect {

	private:
		ID3DX11Effect* m_pEffect;
		ID3DX11EffectTechnique* m_pTechnique;
		ID3DX11EffectShaderResourceVariable* m_pDiffuseVariable;

	public:
		Effect() = default;
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~Effect();
		void UpdateViewProjectionMatrix(const Matrix* viewProjectionMatrix);
		void SetDiffuseMap(Texture* pDiffuseTexture);


		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;
		ID3DX11EffectMatrixVariable* m_pViewProjectionMatrix = nullptr;
	};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

}
