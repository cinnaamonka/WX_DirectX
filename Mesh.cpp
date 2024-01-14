#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
namespace dae
{
	Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices):
		m_pVertexBuffer(nullptr), m_pInputLayout(nullptr)
	{
		// Create Effect
		m_pEffect = new Effect(pDevice, L"./Resources/PosCol3D.fx");

		m_pDiffuseVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

		if (!m_pDiffuseVariable->IsValid())
		{
			std::wcout << L"m_pDiffuseVariable is not valid" << std::endl;
		}

		m_pSpecularVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource(); 

		if (!m_pSpecularVariable->IsValid())
		{
			std::wcout << L"m_pSpecularVariable is not valid" << std::endl;
		}

		m_pGlossinessVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource(); 

		if (!m_pGlossinessVariable->IsValid())
		{
			std::wcout << L"m_pGlossinessVariable is not valid" << std::endl;
		}

		m_pNormalVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource(); 

		if (!m_pNormalVariable->IsValid())
		{
			std::wcout << L"m_pNormalVariable is not valid" << std::endl;
		}

		m_CameraOrigin = m_pEffect->GetVariableByName("gCameraPos")->AsVector();

		if (!m_CameraOrigin->IsValid())
		{
			std::wcout << L"m_CameraOrigin is not valid" << std::endl;
		}

		// Create Vertex Layout
		// Create Vertex Layout
		static constexpr uint32_t numElements{ 5 }; // Update to include normal and tangent
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};
		       
		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "TEXCOORD";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "NORMAL";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; // Adjust the offset based on your structure
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[4].SemanticName = "TANGENT";
		vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; // Adjust the offset based on your structure
		vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		// Create Vertex Buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = vertices.data();

		HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result))
			return;

		D3DX11_PASS_DESC passDesc{};
		result = m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
		if (FAILED(result))
			return;

		result = pDevice->CreateInputLayout(
			vertexDesc,
			numElements,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_pInputLayout);

		if (FAILED(result))
			return;

		m_NumIndices = static_cast<uint32_t>(indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = indices.data();
		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result))
			return;
	}
	bool Mesh::SetDiffuseMap(Texture* pDiffuseTexture)
	{
		if (pDiffuseTexture)
		{
			m_pDiffuseVariable->SetResource(pDiffuseTexture->GetShaderResourceView());
			return true;
		}
		return false;
	}
	bool Mesh::SetSpecularMap(Texture* pSpecularTexture)
	{
		if (pSpecularTexture && m_pSpecularVariable->IsValid())
		{
			m_pSpecularVariable->SetResource(pSpecularTexture->GetShaderResourceView());
			return true;
		}
		return false;
	}

	bool Mesh::SetGlossinessMap(Texture* pGlossinessTexture)
	{
		if (pGlossinessTexture && m_pGlossinessVariable->IsValid())
		{
			m_pGlossinessVariable->SetResource(pGlossinessTexture->GetShaderResourceView());
			return true;
		}
		return false;
	}

	bool Mesh::SetNormalMap(Texture* pNormalTexture)
	{
		if (pNormalTexture && m_pNormalVariable->IsValid())
		{
			m_pNormalVariable->SetResource(pNormalTexture->GetShaderResourceView());
			return true;
		}
		return false;
	}

	Mesh::~Mesh()
	{
		

		// Release Shader Resource Variables
		if (m_pDiffuseVariable)
		{
			m_pDiffuseVariable->Release();
			m_pDiffuseVariable = nullptr;
		}

		if (m_pSpecularVariable)
		{
			m_pSpecularVariable->Release();
			m_pSpecularVariable = nullptr;
		}

		if (m_pGlossinessVariable)
		{
			m_pGlossinessVariable->Release();
			m_pGlossinessVariable = nullptr;
		}

		if (m_pNormalVariable)
		{
			m_pNormalVariable->Release();
			m_pNormalVariable = nullptr;
		}

		// Release other resources
		if (m_pInputLayout)
		{
			m_pInputLayout->Release();
			m_pInputLayout = nullptr;
		}

		if (m_pVertexBuffer)
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = nullptr;
		}

		if (m_pIndexBuffer)
		{
			m_pIndexBuffer->Release();
			m_pIndexBuffer = nullptr;
		}

		// Release Effect resources
		if (m_pEffect)
		{
			delete m_pEffect;
			m_pEffect = nullptr;
		}
	}


	void Mesh::Render(ID3D11DeviceContext* pDeviceContext, const Matrix* viewProjectionMatrix, const Matrix* worldMatrix) const
	{
		m_pEffect->UpdateViewProjectionMatrix(viewProjectionMatrix);
		m_pEffect->UpdateWorldMatrix(worldMatrix);

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechnique()->GetDesc(&techDesc);
		
		if (m_SampleState < techDesc.Passes)
		{
			m_pEffect->GetTechnique()->GetPassByIndex(m_SampleState)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
			
		}
		
	}

};
