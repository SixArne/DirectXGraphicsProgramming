#pragma once

#include <DirectXMath.h>
#include "Effect.h"

struct Vec3
{
	float x{};
	float y{};
	float z{};
};

struct Vertex {
	Vec3 position {};
	Vec3 color{};
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(ID3D11Device* pDevice, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	void Render(ID3D11DeviceContext* pDeviceContext) const;
	~Mesh();

private:
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pEffectTechnique{};
	ID3D11InputLayout* m_pInputLayout{};

	uint32_t m_NumIndices{};
};

