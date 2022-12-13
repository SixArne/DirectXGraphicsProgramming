#pragma once

#include <SDL_surface.h>
#include "d3dx11effect.h"
#include <string>
#include "ColorRGB.h"

namespace dae
{
	struct Vector2;

	class Texture
	{
	public:
		~Texture();

		static Texture* LoadFromFile(ID3D11Device* device, const std::string& path);
		ID3D11ShaderResourceView* GetSRV();

	private:
		Texture() = default;
		ID3D11Texture2D* m_pResource{};
		ID3D11ShaderResourceView* m_pResourceView{};
	};
}