#include "pch.h"
#include "Mesh.h"
#include "Effect.h"

Mesh::Mesh(ID3D11Device* pDevice, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	m_pEffect = new Effect(pDevice, std::wstring{ L"Resources/PosCol3D.fx" });
	m_pEffectTechnique = m_pEffect->GetTechnique();

	// Vertex layout
	const uint32_t numElements{ 3 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	// Position
	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Color
	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// UV coordinates
	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Create vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = vertices.data();

	HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create buffer for mesh");
		return;
	}

	// Create input layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout
	);

	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create input layout");
		return;
	}

	// Create index buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	// Re-use
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create input layout");
		return;
	}
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, dae::Matrix& wvp) const
{
	// Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set input layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// Set vertex buffer
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	ID3D11Buffer* pVertexBuffer = m_pVertexBuffer;

	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	m_pEffect->GetMatrixVariable()->SetMatrix((float*)(&wvp));

	// Set indexBuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p{}; p < techDesc.Passes; p++)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::SetTexture(dae::Texture* texture)
{
	m_pEffect->SetDiffuseMap(texture);
}

Mesh::~Mesh()
{
	m_pVertexBuffer->Release();
	m_pIndexBuffer->Release();
	m_pInputLayout->Release();
	delete m_pEffect;
}
