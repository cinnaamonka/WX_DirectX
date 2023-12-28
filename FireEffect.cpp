#include "pch.h"
#include "FireEffect.h"

namespace dae
{
    FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
    {
            const std::ifstream asset(assetFile);

            if (!asset)
            {
                std::wcout << "File does not exist" << std::endl;
            }

        // Get technique and matrix variables
        m_pTechnique = m_pEffect->GetTechniqueByName("FireTechnique");
        m_pViewProjectionMatrix = m_pEffect->GetVariableByName("gViewProjection")->AsMatrix();
        m_pWorldMatrix = m_pEffect->GetVariableByName("gWorld")->AsMatrix();

        // Get the diffuse map variable for fire effect
        m_pDiffuseMap = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
    }

    FireEffect::~FireEffect()
    {
        // Release resources
        if (m_pEffect)
            m_pEffect->Release();
    }

    void FireEffect::UpdateViewProjectionMatrix(const Matrix* viewProjectionMatrix)
    {
        // Update the viewProjectionMatrix variable in the shader
        if (m_pViewProjectionMatrix)
            m_pViewProjectio
