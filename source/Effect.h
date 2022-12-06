#pragma once
class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	ID3DX11EffectTechnique* GetTechnique();
	ID3DX11Effect* GetEffect();
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

private:
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pEffectTechnique{};
};

