//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibRenderer)

unique_ptr<class LibConstBuffer> MakeConstBuffer(uint bufferSize);
unique_ptr<class LibDepthStencil> MakeDepthStencil(uint width, uint height);
unique_ptr<class LibIndexBuffer> MakeIndexBuffer(uint bytes, const ubyte* data, IndexType indexSize);
unique_ptr<class LibMaterial> MakeMaterial(const DataFile& file);
unique_ptr<class LibPrimitive> MakePrimitive(PrimType type, uint _drawCount, uint _drawOffset);
unique_ptr<class LibRasterState> MakeRasterState(const bool backCull, const bool wireframe);
unique_ptr<class LibBlendState> MakeBlendState(const bool alphaBlend);
unique_ptr<class LibDepthState> MakeDepthState(const bool writeDepth);
unique_ptr<class LibRenderTarget> MakeRenderTarget(uint& width, uint& height);
unique_ptr<class LibSampler> MakeSampler(bool addressWrap, bool comparison, bool filtered);
unique_ptr<class LibTexture> MakeTexture(uint width, uint height, TextureFormat format);
unique_ptr<class LibVertBuffer> MakeVertBuffer(uint bytes, const ubyte* data, uint _stride);
unique_ptr<class LibVertDesc> MakeVertDesc(const vector<VertType::Bit>& bits, const vector<uint>& offsets);
unique_ptr<class LibVertLayout> MakeVertLayout(const unique_ptr<LibVertDesc>& vertDesc, const unique_ptr<LibMaterial>& material);

void Present();
void SetViewport(uint width, uint height, uint x, uint y);
void BindRenderTargets(const vector<LibRenderTarget*>& targets, const unique_ptr<LibDepthStencil>& depth);
void UnbindRenderTargets();
void ResizeBuffers();

};
