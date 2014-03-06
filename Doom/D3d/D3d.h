//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibRenderer::Impl
{
	friend class LibRenderer;
public:
	explicit Impl(HWND window, bool enableD2d)
		:d3d()
	{
		CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory.ToCreate()));
		if (!dxgiFactory)
		{
			THROW("CreateDXGIFactory1 failed.");
		}

		dxgiFactory->EnumAdapters1(0, adapter.ToCreate());

		uint flags = enableD2d ? D3D11_CREATE_DEVICE_BGRA_SUPPORT : 0;
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL level;
		D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_10_0 };
		D3D11CreateDevice(adapter.Ptr(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, levels, sizeof(levels) / sizeof(*levels),
			D3D11_SDK_VERSION, d3d.device.ToCreate(), &level, d3d.context.ToCreate());

		if (!d3d.device || !d3d.context)
		{
			THROW("D3D11CreateDevice failed.");
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = window;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = SwapChainFlags();

		dxgiFactory->CreateSwapChain(d3d.device.Ptr(), &swapChainDesc, swapChain.ToCreate());
		if (!swapChain)
		{
			THROW("CreateSwapChain failed.");
		}

#ifdef _DEBUG
		ComPtr<ID3D11InfoQueue> infoQueue;
		d3d.device->QueryInterface(infoQueue.ToCreate());
		if (!infoQueue)
		{
			THROW("ID3D11InfoQueue failed.");
		}

		D3D11_MESSAGE_ID messageIDs[] = { D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
			D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD };

		D3D11_INFO_QUEUE_FILTER filter = { 0 };
		filter.DenyList.NumIDs = sizeof(messageIDs) / sizeof(*messageIDs);
		filter.DenyList.pIDList = messageIDs;
		infoQueue->AddStorageFilterEntries(&filter);
#endif
	}

private:
	UINT SwapChainFlags()
	{
		return 0;
	}

	D3dObjs d3d;
	ComPtr<IDXGIFactory1> dxgiFactory;
	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<IDXGISwapChain> swapChain;
};

inline void LibRenderer::Present()
{
	impl->swapChain->Present(1, 0);
}

inline void LibRenderer::SetViewport(uint width, uint height, uint x, uint y)
{
	D3D11_VIEWPORT vp;

	vp.MinDepth = D3D11_MIN_DEPTH;
	vp.MaxDepth = D3D11_MAX_DEPTH;
	vp.TopLeftX = static_cast<FLOAT>(x);
	vp.TopLeftY = static_cast<FLOAT>(y);
	vp.Width = static_cast<FLOAT>(width);
	vp.Height = static_cast<FLOAT>(height);

	impl->d3d.context->RSSetViewports(1, &vp);
}

inline void LibRenderer::BindRenderTargets(const vector<LibRenderTarget*>& targets, const unique_ptr<LibDepthStencil>& depth)
{
	vector<ID3D11RenderTargetView*> dxTargets(targets.size());
	for (uint i = 0; i < targets.size(); ++i)
	{
		LibRenderTarget* libTarget = targets[i];
		assert(libTarget);

		dxTargets[i] = libTarget->GetImpl().GetTargetView().Ptr();
	}

	ID3D11DepthStencilView* dxDepth = nullptr;
	if (depth)
	{
		dxDepth = depth->GetImpl().GetView().Ptr();
	}

	impl->d3d.context->OMSetRenderTargets(static_cast<UINT>(dxTargets.size()), dxTargets.data(), dxDepth);
}

inline void LibRenderer::UnbindRenderTargets()
{
	impl->d3d.context->OMSetRenderTargets(0, nullptr, nullptr);
}

inline void LibRenderer::ResizeBuffers()
{
	HRESULT hr = impl->swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, impl->SwapChainFlags());

	if (hr != S_OK)
	{
		THROW("ResizeBuffers failed.");
	}
}

inline unique_ptr<LibConstBuffer> LibRenderer::MakeConstBuffer(uint bufferSize)
{
	return make_unique<LibConstBuffer>(make_unique<LibConstBuffer::Impl>(impl->d3d, bufferSize));
}

inline unique_ptr<LibDepthStencil> LibRenderer::MakeDepthStencil(uint width, uint height)
{
	return make_unique<LibDepthStencil>(make_unique<LibDepthStencil::Impl>(impl->d3d, width, height));
}

inline unique_ptr<LibIndexBuffer> LibRenderer::MakeIndexBuffer(uint bytes, const ubyte* data, IndexType indexSize)
{
	return make_unique<LibIndexBuffer>(make_unique<LibIndexBuffer::Impl>(impl->d3d, bytes, data, indexSize));
}

inline unique_ptr<LibMaterial> LibRenderer::MakeMaterial(const DataFile& file)
{
	return make_unique<LibMaterial>(make_unique<LibMaterial::Impl>(impl->d3d, file));
}

inline unique_ptr<LibPrimitive> LibRenderer::MakePrimitive(PrimType type, uint _drawCount, uint _drawOffset)
{
	return make_unique<LibPrimitive>(make_unique<LibPrimitive::Impl>(impl->d3d, type, _drawCount, _drawOffset));
}

inline unique_ptr<LibRasterState> LibRenderer::MakeRasterState(const bool backCull, const bool wireframe)
{
	return make_unique<LibRasterState>(make_unique<LibRasterState::Impl>(impl->d3d, backCull, wireframe));
}

inline unique_ptr<class LibBlendState> LibRenderer::MakeBlendState(const bool alphaBlend)
{
	return make_unique<LibBlendState>(make_unique<LibBlendState::Impl>(impl->d3d, alphaBlend));
}

inline unique_ptr<class LibDepthState> LibRenderer::MakeDepthState(const bool writeDepth)
{
	return make_unique<LibDepthState>(make_unique<LibDepthState::Impl>(impl->d3d, writeDepth));
}

inline unique_ptr<LibRenderTarget> LibRenderer::MakeRenderTarget(uint& width, uint& height)
{
	return make_unique<LibRenderTarget>(make_unique<LibRenderTarget::Impl>(impl->d3d, impl->swapChain, width, height));
}

inline unique_ptr<LibSampler> LibRenderer::MakeSampler(bool addressWrap, bool comparison, bool filtered)
{
	return make_unique<LibSampler>(make_unique<LibSampler::Impl>(impl->d3d, addressWrap, comparison, filtered));
}

inline unique_ptr<LibTexture> LibRenderer::MakeTexture(uint width, uint height, TextureFormat format)
{
	return make_unique<LibTexture>(make_unique<LibTexture::Impl>(impl->d3d, width, height, format));
}

inline unique_ptr<LibVertBuffer> LibRenderer::MakeVertBuffer(uint bytes, const ubyte* data, uint _stride)
{
	return make_unique<LibVertBuffer>(make_unique<LibVertBuffer::Impl>(impl->d3d, bytes, data, _stride));
}

inline unique_ptr<LibVertDesc> LibRenderer::MakeVertDesc(const vector<VertType::Bit>& bits, const vector<uint>& offsets)
{
	return make_unique<LibVertDesc>(make_unique<LibVertDesc::Impl>(impl->d3d, bits, offsets));
}

inline unique_ptr<LibVertLayout> LibRenderer::MakeVertLayout(const unique_ptr<LibVertDesc>& vertDesc, const unique_ptr<LibMaterial>& material)
{
	return make_unique<LibVertLayout>(make_unique<LibVertLayout::Impl>(impl->d3d, vertDesc, material));
}
