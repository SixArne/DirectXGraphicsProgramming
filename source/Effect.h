#pragma once
#include "Texture.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	ID3DX11EffectTechnique* GetTechnique();
	ID3DX11Effect* GetEffect();
	ID3DX11EffectMatrixVariable* GetMatrixVariable();
	void SetDiffuseMap(dae::Texture* pDiffuseMap);
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

private:
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pEffectTechnique{};
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
};

