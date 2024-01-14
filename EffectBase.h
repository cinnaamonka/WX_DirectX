#pragma once
#include "Texture.h"

namespace dae
{
	class EffectBase
	{

	private:
		ID3DX11Effect* m_pEffect = nullptr;
		ID3DX11EffectTechnique* m_pTechnique = nullptr;
		ID3DX11EffectMatrixVariable* m_pViewProjectionMatrix = nullptr;
		ID3DX11EffectMatrixVariable* m_pWorldMatrix = nullptr;

	public:
		EffectBase() = default;
		EffectBase(ID3D11Device* pDevice, const std::wstring& assetFile,std::string techiqueName);
		~EffectBase();
		void UpdateViewProjectionMatrix(const Matrix* viewProjectionMatrix);
		void UpdateWorldMatrix(const Matrix* worldMatrix);

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;

		void SetViewProjectionMatrix(float* viewProjectionMatrix)
		{
			m_pViewProjectionMatrix->SetMatrix(viewProjectionMatrix);
		}
		
		LPSTR CreateLPSTRFromConstCharArray(const char* constCharArray)
		{
			if (constCharArray == nullptr) {
				return nullptr;
			}

			// Calculate the length of the source string
			size_t length = std::strlen(constCharArray);

			// Allocate memory for the non-const copy.
			LPSTR nonConstStr = new char[length + 1];

			// Copy the content using std::copy
			std::copy(constCharArray, constCharArray + length, nonConstStr);

			// Ensure null-termination.
			nonConstStr[length] = '\0';

			return nonConstStr;
		}
		ID3DX11EffectMatrixVariable* GetViewProjectionMatrix()
		{
			return m_pViewProjectionMatrix;
		}
		ID3DX11EffectVariable* GetVariableByName(const std::string& name) const;
	};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

}


