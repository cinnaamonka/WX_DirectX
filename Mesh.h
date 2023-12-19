#pragma once
#include "pch.h"
#include "Effect.h"

class Effect;
class Matrix;
class Texture;

namespace dae
{
	struct Vertex
	{
		Vector3  position = { 0.0f,0.0f, 0.0f };
		ColorRGB color = colors::White;
		Vector2  uv = { 0.0f, 1.0f };
		Vector3  normal = { 0.0f, 0.0f, 1.0f };
		Vector3  tangent = { 0.0f, 0.0f, 1.0f };
	};

	class Mesh
	{
		Effect* m_pEffect;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11InputLayout* m_pInputLayout;
		int m_NumIndices;
		ID3D11Buffer* m_pIndexBuffer;
		ID3DX11EffectShaderResourceVariable* m_pDiffuseVariable = nullptr;

		

	public:
		Mesh() = default;
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();
		void UpdateWorldmatrix();
		bool SetDiffuseMap(Texture* pDiffuseTexture);
		void Render(ID3D11DeviceContext* pDeviceContext, const Matrix* viewProjectionMatrix, Texture* myTexture) const;

		void SetSampleState(UINT stateIndex) 
		{ 
			m_SampleState = stateIndex; 
		};

		void SetMatrix(const Matrix& viewMatrix, const Matrix& projectionMatrix) 
		{
			m_ViewMatrix = viewMatrix;
			m_ProjectionMatrix = projectionMatrix;
			Matrix worldViewProjectionMatrix = m_WorldMatrix * m_ViewMatrix * m_ProjectionMatrix;
			m_pEffect->SetViewProjectionMatrix(reinterpret_cast<float*>(&worldViewProjectionMatrix)); 
			//m_pEffect->UpdateViewProjectionMatrix(&worldViewProjectionMatrix);
		}
		void RotateY(float yaw)
		{
			m_WorldMatrix = Matrix::CreateRotationY(yaw);
		}
		void Translate(float posZ)
		{
			m_WorldMatrix = Matrix::CreateTranslation(Vector3(0, 0, posZ));

		}
		Matrix GetWorldMatrix()
		{
			return m_WorldMatrix;
		}

	private:
		UINT m_SampleState;
		Matrix m_WorldMatrix{};
		Matrix m_ViewMatrix{};
		Matrix m_ProjectionMatrix{};


	};
}
