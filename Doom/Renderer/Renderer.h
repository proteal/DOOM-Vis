//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class Renderer
{
public:
	explicit Renderer(unique_ptr<LibRenderer>& libRenderer)
		:rGlobal(libRenderer)
		, frameData()
		, constBuffers()
		, vertDescs()
		, samplers()
		, materials(samplers, constBuffers)
		, rasterStates()
		, blendStates()
		, depthStates()
		, screenQuad(vertDescs)
		, wallMesh(vertDescs)
		, camMesh(vertDescs)
		, lineMesh(vertDescs)
	{
		XMMATRIX projectionA = XMMatrixPerspectiveFovRH(XM_PI * 0.3f, 1.9f, 1.0f, 10000.0f);
		XMStoreFloat4x4(&projection, projectionA);

		MakeBackBufferTarget();

		const shared_ptr<Material>& outputMat = materials.GetMaterial(L"output.hlsl");
		outputMatInst = make_unique<MaterialInst>(outputMat);

		emuTexture = make_shared<Texture>(RendererDefs::DOOM_WIDTH,
			RendererDefs::DOOM_HEIGHT, TextureFormat::R8_G8_B8_A8_UNORM_SRGB);

		shared_ptr<ShaderVarTexture> emuTextureVar = outputMatInst->GetTextureVar(L"emuTexture");
		assert(emuTextureVar);
		emuTextureVar->SetTexture(emuTexture);

		emuTextureWriter = make_unique<TextureWriter>(emuTexture);

		const shared_ptr<Material>& wallMat = materials.GetMaterial(L"wall.hlsl");
		wallMatInst = make_unique<MaterialInst>(wallMat);

		worldViewProjVar = wallMatInst->GetShaderVar<ShaderVarFloat4x4>(L"worldViewProj");
		worldVar = wallMatInst->GetShaderVar<ShaderVarFloat4x4>(L"world");
		playerPosVar = wallMatInst->GetShaderVar<ShaderVarFloat3>(L"playerPos");

		const shared_ptr<Material>& planeMat = materials.GetMaterial(L"plane.hlsl");
		planeMatInst = make_unique<MaterialInst>(planeMat);

		planeWorldViewProjVar = planeMatInst->GetShaderVar<ShaderVarFloat4x4>(L"planeWorldViewProj");
		planeColorVar = planeMatInst->GetShaderVar<ShaderVarFloat3>(L"planeColor");

		const shared_ptr<Material>& camMat = materials.GetMaterial(L"cam.hlsl");
		camMatInst = make_unique<MaterialInst>(camMat);

		camWorldViewProjVar = camMatInst->GetShaderVar<ShaderVarFloat4x4>(L"camWorldViewProj");
	}

	void OnResize()
	{
		RenderGlobal::Get().GetLib().UnbindRenderTargets();
		backBuffer = nullptr;
		depthStencil = nullptr;

		RenderGlobal::Get().GetLib().ResizeBuffers();
		MakeBackBufferTarget();
	}

	void DrawFrame()
	{
		assert(backBuffer);
		assert(depthStencil);

		vector<LibRenderTarget*> renderTargets = { backBuffer->GetLibTarget().get() };
		RenderGlobal::Get().GetLib().BindRenderTargets(renderTargets, depthStencil->GetLibDepthStencil());
		backBuffer->ClearTarget({ 0.0f, 0.0f, 0.0f, 0.0f });
		depthStencil->ClearDepthStencil();

		const FrameDataBuffer& dataBuffer = frameData.BeginReading();
		DrawDoom(dataBuffer);
		DrawDevView(dataBuffer);

		RenderGlobal::Get().GetLib().Present();
	}

	FrameDataWriter& GetFrameData()
	{
		return frameData;
	}

private:
	void MakeBackBufferTarget()
	{
		backBuffer = make_shared<RenderTarget>();

		uint width = backBuffer->GetWidth();
		uint height = backBuffer->GetHeight();
		depthStencil = make_unique<DepthStencil>(width, height);

		float aspect = RendererDefs::TOTAL_WIDTH() / RendererDefs::DOOM_HEIGHT;
		uint vpWidth = min(width, static_cast<uint>(aspect * height + 0.5f));

		viewportDims.width = vpWidth / 2;
		viewportDims.height = min(height, static_cast<uint>(1.0f / aspect * vpWidth + 0.5f));

		viewportDims.leftX = (width - vpWidth) / 2;
		viewportDims.rightX = viewportDims.leftX + viewportDims.width;
		viewportDims.y = (height - viewportDims.height) / 2;
	}

	void DrawDoom(const FrameDataBuffer& dataBuffer)
	{
		assert(emuTextureWriter);
		assert(outputMatInst);

		emuTextureWriter->BeginWrite();

		for (uint i = 0; i < RendererDefs::FRAME_PIXELS; i++)
		{
			const ColorRgb& srcColor = dataBuffer.pixels[i];
			ColorRgba color = { srcColor[0], srcColor[1], srcColor[2], 0xff };

			emuTextureWriter->WriteVal(color);
		}
		emuTextureWriter->EndWrite();

		RenderGlobal::Get().GetLib().SetViewport(viewportDims.width, viewportDims.height, viewportDims.leftX, viewportDims.y);
		rasterStates.GetRasterState(RasterStateType::NORMAL).BindRasterState();

		screenQuad.DrawQuad(outputMatInst);
	}

	void DrawDevView(const FrameDataBuffer& dataBuffer)
	{
		assert(playerPosVar);

		rasterStates.GetRasterState(RasterStateType::TWO_SIDE_WIREFRAME).BindRasterState();
		RenderGlobal::Get().GetLib().SetViewport(viewportDims.width, viewportDims.height, viewportDims.rightX, viewportDims.y);

		XMVECTOR viewPos = XMLoadFloat3(&dataBuffer.viewPos);
		XMVECTOR viewDir = XMLoadFloat3(&dataBuffer.viewDir);

		XMMATRIX viewMat = XMMatrixLookToRH(viewPos, viewDir, g_XMIdentityR2);
		XMMATRIX projectionA = XMLoadFloat4x4(&projection);
		XMMATRIX viewProj = XMMatrixMultiply(viewMat, projectionA);

		DrawCam(dataBuffer, viewProj);

		rasterStates.GetRasterState(RasterStateType::TWO_SIDE).BindRasterState();
		playerPosVar->SetVarData(dataBuffer.playerPos);

		for (size_t i = 0; i < dataBuffer.segs.size(); i++)
		{
			const DoomSeg& seg = dataBuffer.segs[i];
			DrawWall(seg, viewProj);
		}

		const float planeZ = dataBuffer.playerPos.z - 41.0f;
		for (size_t i = 0; i < dataBuffer.nodes.size(); i++)
		{
			const DoomBspNode& node = dataBuffer.nodes[i];
			DrawPlane(planeZ, node, viewProj);
		}
	}

	void DrawCam(const FrameDataBuffer& dataBuffer, const XMMATRIX& viewProj)
	{
		assert(camWorldViewProjVar);

		XMMATRIX camRot = XMMatrixRotationZ(dataBuffer.playerAngle);
		XMMATRIX camMat = XMMatrixTranslation(dataBuffer.playerPos.x, dataBuffer.playerPos.y, dataBuffer.playerPos.z);
		camMat = XMMatrixMultiply(camRot, camMat);
		camMat = XMMatrixMultiply(camMat, viewProj);

		camWorldViewProjVar->SetVarData(camMat);
		camMesh.DrawCam(camMatInst);
	}

	void DrawWall(const DoomSeg& seg, const XMMATRIX& viewProj)
	{
		assert(worldViewProjVar);
		assert(worldVar);

		XMVECTOR v1 = XMLoadFloat2(&seg.v1);
		XMVECTOR v2 = XMLoadFloat2(&seg.v2);
		XMVECTOR v1v2 = XMVectorSubtract(v2, v1);
		float wallLen = XMVectorGetX(XMVector2Length(v1v2));

		float wallAngle = XMVectorGetX(XMVector2AngleBetweenVectors(v1v2, g_XMIdentityR0));
		if (XMVectorGetY(v1v2) < 0.0f)
		{
			wallAngle = -wallAngle;
		}
		XMMATRIX rot = XMMatrixRotationZ(wallAngle);

		XMMATRIX scaling = XMMatrixScaling(wallLen, 1.0f, seg.ceilingZ - seg.floorZ);
		XMMATRIX worldMat = XMMatrixMultiply(scaling, rot);

		v1 = XMVectorSetZ(v1, seg.floorZ);
		XMMATRIX trans = XMMatrixTranslationFromVector(v1);

		worldMat = XMMatrixMultiply(worldMat, trans);
		worldVar->SetVarData(worldMat);

		XMMATRIX worldViewProj = XMMatrixMultiply(worldMat, viewProj);
		worldViewProjVar->SetVarData(worldViewProj);

		wallMesh.DrawWall(wallMatInst);
	}

	void DrawPlane(const float planeZ, const DoomBspNode& node, const XMMATRIX& viewProj)
	{
		assert(planeWorldViewProjVar);
		assert(planeColorVar);

		const XMVECTOR v1 = XMLoadFloat2(&node.v1);
		const XMVECTOR v2 = XMLoadFloat2(&node.v2);
		XMVECTOR v1v2 = XMVectorSubtract(v2, v1);
		float wallLen = XMVectorGetX(XMVector2Length(v1v2));

		float wallAngle = XMVectorGetX(XMVector2AngleBetweenVectors(v1v2, g_XMIdentityR0));
		if (XMVectorGetY(v1v2) < 0.0f)
		{
			wallAngle = -wallAngle;
		}
		XMMATRIX rot = XMMatrixRotationZ(wallAngle);

		XMMATRIX scaling = XMMatrixScaling(wallLen, 1.0f, 1.0f);
		XMMATRIX worldMat = XMMatrixMultiply(scaling, rot);

		XMVECTOR transVec = XMVectorSetZ(v1, planeZ);
		XMMATRIX trans = XMMatrixTranslationFromVector(transVec);
		worldMat = XMMatrixMultiply(worldMat, trans);

		XMMATRIX worldViewProj = XMMatrixMultiply(worldMat, viewProj);
		planeWorldViewProjVar->SetVarData(worldViewProj);

		XMFLOAT3 color = node.twoSide ? XMFLOAT3{ 0.0f, 0.75f, 0.0f } : XMFLOAT3{ 0.75f, 0.75f, 0.0f };
		planeColorVar->SetVarData(color);

		lineMesh.DrawLine(planeMatInst);
	}

	RenderGlobal rGlobal;
	FrameData frameData;
	VertDescs vertDescs;
	Samplers samplers;
	ConstBuffers constBuffers;
	Materials materials;
	RasterStates rasterStates;
	BlendStates blendStates;
	DepthStates depthStates;
	shared_ptr<RenderTarget> backBuffer;
	unique_ptr<DepthStencil> depthStencil;
	ScreenQuad screenQuad;
	WallMesh wallMesh;
	CamMesh camMesh;
	LineMesh lineMesh;
	shared_ptr<Texture> emuTexture;
	ViewportDims viewportDims;
	XMFLOAT4X4 projection;
	shared_ptr<ShaderVarFloat4x4> worldViewProjVar;
	shared_ptr<ShaderVarFloat4x4> worldVar;
	shared_ptr<ShaderVarFloat4x4> planeWorldViewProjVar;
	shared_ptr<ShaderVarFloat3> planeColorVar;
	shared_ptr<ShaderVarFloat4x4> camWorldViewProjVar;
	shared_ptr<ShaderVarFloat3> playerPosVar;
	unique_ptr<TextureWriter> emuTextureWriter;
	unique_ptr<MaterialInst> outputMatInst;
	unique_ptr<MaterialInst> wallMatInst;
	unique_ptr<MaterialInst> camMatInst;
	unique_ptr<MaterialInst> planeMatInst;
};
