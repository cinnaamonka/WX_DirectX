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
		//Vector3  normal = { 0.0f, 0.0f, 1.0f };
		//Vector3  tangent = { 0.0f, 0.0f, 1.0f };
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
		bool SetDiffuseMap(Texture* pDiffuseTexture);
		void Render(ID3D11DeviceContext* pDeviceContext, const Matrix* viewProjectionMatrix, Texture* myTexture) const;
	};
}
