#pragma once

struct SDL_Window;
struct SDL_Surface;

#include "Mesh.h"
#include "Camera.h"

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render() const;

	private:
		SDL_Window* m_pWindow{};

		// DirectX members
		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		IDXGISwapChain* m_pSwapChain{};
		//ID3D11Resource* m_pDepthStencilBuffer{};
		//ID3D11RenderTargetView* m_pDepthStencilView{};

		// Stencil
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};

		// Back buffer
		ID3D11Texture2D* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};

		Mesh* m_pMesh;
		Camera* m_pCamera{};
		Texture* m_pDiffuseTexture{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		//DIRECTX
		HRESULT InitializeDirectX();
		//...
	};
}
