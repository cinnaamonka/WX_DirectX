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

		// Create Vertex Layout
		static constexpr uint32_t numElements{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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
	Mesh::~Mesh()
	{
		// TODO: initialize destruct sequence
		if (m_pIndexBuffer) m_pIndexBuffer->Release();
		if (m_pInputLayout) m_pInputLayout->Release();
		if (m_pVertexBuffer) m_pVertexBuffer->Release();
		delete m_pEffect;
		m_pEffect = nullptr;
		if (m_pDiffuseVariable) m_pDiffuseVariable->Release();
	}

	void Mesh::Render(ID3D11DeviceContext* pDeviceContext, const Matrix* viewProjectionMatrix,Texture* myTexture) const
	{
		m_pEffect->UpdateViewProjectionMatrix(viewProjectionMatrix);

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechnique()->GetDesc(&techDesc);
		
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
		
			m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
			
		}
	}
};
