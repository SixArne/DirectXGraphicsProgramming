#include "pch.h"

#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>

#include <stdexcept>

namespace dae
{
	Texture::~Texture()
	{
		if (m_pResource != nullptr)
		{
			m_pResource->Release();
		}

		if (m_pResourceView != nullptr)
		{
			m_pResourceView->Release();
		}
	}

	Texture* Texture::LoadFromFile(ID3D11Device* device, const std::string& path)
	{
		SDL_Surface* pTextureSurface = IMG_Load(path.data());
		if (pTextureSurface == NULL)
		{
			throw std::runtime_error("Error, texture file not found");
		}

		Texture* newTexture = new Texture();

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pTextureSurface->w;
		desc.Height = pTextureSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = pTextureSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pTextureSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pTextureSurface->h * pTextureSurface->pitch);

		HRESULT hr = device->CreateTexture2D(&desc, &initData, &newTexture->m_pResource);
		if (FAILED(hr))
		{
			// Clean up texture
			delete newTexture;
			throw std::runtime_error("Failed to create texture");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(newTexture->m_pResource, &SRVDesc, &newTexture->m_pResourceView);
		if (FAILED(hr))
		{
			// Clean up texture
			delete newTexture;
			throw std::runtime_error("Failed to create resource view for texture");
		}

		SDL_FreeSurface(pTextureSurface);
		return newTexture;
	}

	ID3D11ShaderResourceView* Texture::GetSRV()
	{
		return m_pResourceView;
	}
	
}