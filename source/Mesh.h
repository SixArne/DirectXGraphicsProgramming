#pragma once

#include <DirectXMath.h>
#include "Effect.h"
#include "Matrix.h"
#include "Math.h"
#include "Texture.h"


struct Vertex {
	dae::Vector3 position{};
	dae::Vector3 color{1,1,1};
	dae::Vector2 uv{};
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(ID3D11Device* pDevice, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	void Render(ID3D11DeviceContext* pDeviceContext, dae::Matrix& wvp) const;
	void SetTexture(dae::Texture* texture);
	~Mesh();

private:
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pEffectTechnique{};
	ID3D11InputLayout* m_pInputLayout{};

	uint32_t m_NumIndices{};
};

