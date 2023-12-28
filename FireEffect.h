#pragma once
#include "Texture.h"

namespace dae
{
	class FireEffect
	{

	private:
		ID3DX11Effect* m_pEffect = nullptr;
		ID3DX11EffectTechnique* m_pTechnique = nullptr;
		ID3DX11EffectMatrixVariable* m_pViewProjectionMatrix = nullptr;
		ID3DX11EffectMatrixVariable* m_pWorldMatrix = nullptr;

		// Add the following variables for your new flat shading effect
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMap = nullptr;

	public:
		FireEffect() = default;
		FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~FireEffect();

		void UpdateViewProjectionMatrix(const Matrix* viewProjectionMatrix);
		void UpdateWorldMatrix(const Matrix* worldMatrix);

		// Add a method to set the diffuse map for the flat shading effect
		void SetDiffuseMap(ID3D11ShaderResourceView* diffuseMap);

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;

		// Add a method to get the view projection matrix variable
		ID3DX11EffectMatrixVariable* GetViewProjectionMatrix()
		{
			return m_pViewProjectionMatrix;
		}

		// Add a method to get the diffuse map variable
		ID3DX11EffectShaderResourceVariable* GetDiffuseMap()
		{
			return m_pDiffuseMap;
		}

		// Add a method to get any other shader variable by name if needed
		ID3DX11EffectVariable* GetVariableByName(const std::string& name) const;
	};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

}


