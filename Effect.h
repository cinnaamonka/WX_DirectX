#pragma once
#include "Texture.h"

namespace dae
{
	class Effect {

	private:
		ID3DX11Effect* m_pEffect = nullptr;
		ID3DX11EffectTechnique* m_pTechnique = nullptr;
		ID3DX11EffectMatrixVariable* m_pViewProjectionMatrix = nullptr;
		ID3DX11EffectMatrixVariable* m_pWorldMatrix = nullptr;

		

	public:
		Effect() = default;
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~Effect();
		void UpdateViewProjectionMatrix(const Matrix* viewProjectionMatrix);
		void UpdateWorldMatrix(const Matrix* worldMatrix);

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;
		
		void SetViewProjectionMatrix(float* viewProjectionMatrix)
		{
			m_pViewProjectionMatrix->SetMatrix(viewProjectionMatrix);
		}

		ID3DX11EffectMatrixVariable* GetViewProjectionMatrix()
		{
			return m_pViewProjectionMatrix;
		}
		ID3DX11EffectVariable* GetVariableByName(const std::string& name) const;

		

	};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

}
