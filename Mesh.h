#pragma once
#include "pch.h"
#include "Effect.h"

class Effect;
class Matrix;

namespace dae
{
	struct Vertex_PosCol
	{
		float position[3];
		float color[3];
	};

	class Mesh
	{
		Effect* m_pEffect;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11InputLayout* m_pInputLayout;
		int m_NumIndices;
		ID3D11Buffer* m_pIndexBuffer;

	public:
		Mesh() = default;
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex_PosCol>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();
		void Render(ID3D11DeviceContext* pDeviceContext, const Matrix* viewProjectionMatrix, Texture* myTexture) const;
	};
}
