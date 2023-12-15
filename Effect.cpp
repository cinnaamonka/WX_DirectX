#include "pch.h"
#include "Effect.h"
#include "Texture.h"
#include <fstream>

namespace dae
{

	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		const std::ifstream asset(assetFile);

		if (!asset)
		{
			std::wcout << "File does not exist" << std::endl;
		}
			

		m_pEffect = LoadEffect(pDevice, assetFile);
		m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");

		if (not m_pTechnique->IsValid())
			std::wcout << L"Technique is not valid" << std::endl;

		m_pViewProjectionMatrix = m_pEffect->GetVariableByName("worldViewProjection")->AsMatrix();

		if (!m_pViewProjectionMatrix->IsValid())
		{
			std::wcout << L"ViewProjectionMatrix is not valid" << std::endl;
		}
		
		m_pDiffuseVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

		if (!m_pDiffuseVariable->IsValid())
		{
			std::wcout << L"m_pDiffuseVariable is not valid" << std::endl;
		}
		

	}

	Effect::~Effect()
	{
		m_pEffect->Release();
		m_pEffect = nullptr;
	}
	
	ID3DX11Effect* Effect::GetEffect() const
	{
		return m_pEffect;
	}
	void Effect::UpdateViewProjectionMatrix(const Matrix* viewProjectionMatrix)
	{
		if (m_pEffect)
		{

			if (m_pViewProjectionMatrix)
			{
				Matrix transposedMatrix = Matrix::Transpose(*viewProjectionMatrix);

				const float* data = viewProjectionMatrix->GetMatrixAsArray();

				m_pViewProjectionMatrix->SetMatrix(data);

				delete data;
			}
		}
	}
	ID3DX11EffectTechnique* Effect::GetTechnique() const
	{
		return m_pTechnique;
	}
	void Effect::SetDiffuseMap(Texture* pDiffuseTexture)
	{
		if (m_pDiffuseVariable)
		{
			m_pDiffuseVariable->SetResource(pDiffuseTexture->GetShaderResourceView());
		}
	}
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect;

		DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		result = D3DX11CompileEffectFromFile(assetFile.c_str(),
			nullptr,
			nullptr,
			shaderFlags,
			0,
			pDevice,
			&pEffect,
			&pErrorBlob);

		if (FAILED(result))
		{
			if (pErrorBlob != nullptr)
			{
				const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

				std::wstringstream ss;
				for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
					ss << pErrors[i];

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << std::endl;
			}
			else
			{
				std::wstringstream ss;
				ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
				std::wcout << ss.str() << std::endl;
				return nullptr;
			}
		}
		return pEffect;
	}
	
};
