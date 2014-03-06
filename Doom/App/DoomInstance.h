//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class DoomInstance final : private IDoomCallbacks
{
public:
	explicit DoomInstance(FrameDataWriter& _frameData, InputClient& _input, Audio& _audio)
		:audio(_audio)
		, frameData(_frameData)
		, input(_input)
		, gameThread()
		, frameType(FrameType::NORMAL)
		, frameDataBuffer(nullptr)
		, camDist(0.0f)
		, camYaw(0.0f)
		, camPitch(0.0f)
		, bspStep(0)
		, bspAnimTicks(0)
		, finishedBspAnim(false)
		, doingBspAnim(false)
	{
		doomCallbacks = this;
		gameEnded = false;
		palette.fill({ 0 });
	}

	~DoomInstance()
	{
		InterruptGame();
		doomCallbacks = nullptr;
	}

	void StartGame()
	{
		auto threadEntry = [this](const atomic_bool& endThread)
		{
			ThreadEntry(endThread);
		};
		gameThread.StartThread(threadEntry, nullptr);
	}

	void InterruptGame()
	{
		gameThread.EndThread();
	}

	bool GameEnded() const
	{
		return gameEnded;
	}

private:
	static const uint PALETTE_SIZE = 256;

	void ThreadEntry(const atomic_bool& endThread)
	{
		myargc = 0;
		myargv = nullptr;
		D_DoomMain();

		while (!endThread && !gameEnded)
		{
			frameDataBuffer = &frameData.BeginWriting();
			int ticks;
			frameType = DoDoomFrame(ticks);

			for (uint i = 0; i < RendererDefs::FRAME_PIXELS; i++)
			{
				int paletteIdx = screens[0][i];
				frameDataBuffer->pixels[i] = palette[paletteIdx];
			}

			InputStateBuffer inputState;
			input.GetState().PopState(inputState);

			UpdateCam(ticks, inputState);
			UpdateBspAnim(ticks, inputState);

			frameData.EndWriting();
			frameDataBuffer = nullptr;
		}
	}

	FrameType DoDoomFrame(int& ticks)
	{
		if (frameType == FrameType::NORMAL)
		{
			return D_DoomFrame(ticks);
		}

		assert(frameType == FrameType::WIPE);
		return D_DoomWipeFrame(ticks);
	}

	void UpdateCam(const int ticks, InputStateBuffer& inputState)
	{
		assert(frameDataBuffer);

		if (inputState.GetTrigger(InputTrigger::RESET_CAM))
		{
			camDist = 0.0f;
			camPitch = camYaw = 0.0f;
		}

		if (inputState.focusDevView)
		{
			if (inputState.mouseMoveX != 0)
			{
				camYaw -= AppDefs::DEV_VIEW_TURN_SPEED() * inputState.mouseMoveX;
			}

			if (inputState.mouseMoveY != 0)
			{
				camPitch -= AppDefs::DEV_VIEW_TURN_SPEED() * inputState.mouseMoveY;
			}

			const bool moveForward = inputState.GetButton(InputButton::CAM_FORWARD);
			const bool moveBack = inputState.GetButton(InputButton::CAM_BACK);
			const float speed = AppDefs::DEV_VIEW_MOVE_SPEED() * ticks;

			if (moveForward || moveBack)
			{
				if (moveForward)
				{
					camDist += speed;
				}
				else
				{
					camDist -= speed;
				}
			}
		}

		frameDataBuffer->playerPos = { AppDefs::FixedToFloat(viewx), AppDefs::FixedToFloat(viewy),
			AppDefs::FixedToFloat(viewz) - AppDefs::FixedToFloat(viewzBob) };
		frameDataBuffer->playerAngle = AppDefs::AngleToRadians(viewangle);

		XMVECTOR forward = XMVectorSet(cosf(frameDataBuffer->playerAngle), sinf(frameDataBuffer->playerAngle), 0.0f, 0.0f);
		XMVECTOR right = XMVector3Cross(forward, g_XMIdentityR2);

		XMMATRIX rot = XMMatrixRotationAxis(right, camPitch);
		rot = XMMatrixMultiply(rot, XMMatrixRotationZ(camYaw));

		forward = XMVector3Transform(forward, rot);
		XMVECTOR viewPos = XMLoadFloat3(&frameDataBuffer->playerPos);
		viewPos += camDist * forward;

		XMStoreFloat3(&frameDataBuffer->viewDir, forward);
		XMStoreFloat3(&frameDataBuffer->viewPos, viewPos);
	}

	void UpdateBspAnim(const int ticks, InputStateBuffer& inputState)
	{
		if (inputState.GetTrigger(InputTrigger::TOGGLE_ANIM))
		{
			doingBspAnim = !doingBspAnim;

			if (doingBspAnim)
			{
				bspAnimTicks = 0;
				finishedBspAnim = false;
				bspStep = 0;
			}
		}

		if (!doingBspAnim)
		{
			return;
		}

		bspAnimTicks += ticks;
		int nextStepTicks = finishedBspAnim ? AppDefs::BSP_ANIM_HOLD_TICKS : AppDefs::TICKS_PER_BSP_STEP;

		if (bspAnimTicks >= nextStepTicks)
		{
			int steps = bspAnimTicks / nextStepTicks;
			bspAnimTicks -= steps * nextStepTicks;

			if (finishedBspAnim)
			{
				finishedBspAnim = false;
				bspStep = 0;
			}
			else
			{
				bspStep += steps;
			}
		}
	}

	bool StepBsp(int curNode, uint& curSeg, int& stepsLeft, const vector<BspNodeInfo>& nodes, const vector<SegInfo>& segs,
		vector<bool>& nodesActive, vector<bool>& segsActive)
	{
		if (!stepsLeft)
		{
			return false;
		}

		const BspNodeInfo& node = nodes[curNode];
		nodesActive[curNode] = true;
		stepsLeft--;

		StepChild(curNode, node.frontChild, curSeg, stepsLeft, nodes, segs, nodesActive, segsActive);
		StepChild(curNode, node.backChild, curSeg, stepsLeft, nodes, segs, nodesActive, segsActive);

		if (!stepsLeft)
		{
			return false;
		}

		nodesActive[curNode] = false;
		stepsLeft--;
		return true;
	}

	void StepChild(int parent, int curNode, uint& curSeg, int& stepsLeft, const vector<BspNodeInfo>& nodes, const vector<SegInfo>& segs,
		vector<bool>& nodesActive, vector<bool>& segsActive)
	{
		if (curNode == AppDefs::NULL_NODE)
		{
			StepSeg(parent, curSeg, stepsLeft, segs, segsActive);
		}
		else
		{
			StepBsp(curNode, curSeg, stepsLeft, nodes, segs, nodesActive, segsActive);
		}
	}

	void StepSeg(int curNode, uint& curSeg, int& stepsLeft, const vector<SegInfo>& segs, vector<bool>& segsActive)
	{
		if (!stepsLeft)
		{
			return;
		}

		int nextStepsLeft = stepsLeft - 1;
		while (curSeg < segs.size())
		{
			const SegInfo& seg = segs[curSeg];
			if (seg.parent == curNode)
			{
				segsActive[curSeg++] = true;
				stepsLeft = nextStepsLeft;
			}
			else
			{
				break;
			}
		}
	}

	void AddSeg(const seg_t& seg, const fixed_t& floor, const fixed_t& ceiling)
	{
		if (floor == ceiling)
		{
			return;
		}

		DoomSeg drawnSeg;
		AppDefs::VertToFloat2(*seg.v1, drawnSeg.v1);
		AppDefs::VertToFloat2(*seg.v2, drawnSeg.v2);

		drawnSeg.floorZ = AppDefs::FixedToFloat(floor);
		drawnSeg.ceilingZ = AppDefs::FixedToFloat(ceiling);

		frameDataBuffer->segs.push_back(drawnSeg);
	}

	void Printf(const char* str, ...) override
	{
		str;
#if USE_LOG
		va_list args;
		va_start(args, str);

		char buffer[1024];
		vsprintf_s(buffer, str, args);

		Global::Get().DebugLog(buffer);
		va_end(args);
#endif
	}

	int64 GetMicroSecs() override
	{
		auto now = Global::Get().GetClock().Now();
		return chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();
	}

	void SetPalette(const ubyte* _palette) override
	{
		const ColorRgb* srcPalette = reinterpret_cast<const ColorRgb*>(_palette);
		copy(srcPalette, srcPalette + PALETTE_SIZE, palette.begin());
	}

	void Sleep() override
	{
		this_thread::sleep_for(chrono::milliseconds(15));
	}

	void EndGame() override
	{
		gameEnded = true;
	}

	void PopInputEvents(vector<event_t>& buffer) override
	{
		input.GetEvents().PopEvents(buffer);
	}

	void SetUsingCursor(bool usingCursor) override
	{
		input.SetUsingCursor(usingCursor);
	}

	void OnSeg(const seg_t& seg)
	{
		assert(seg.v1);
		assert(seg.v2);
		assert(seg.sidedef);
		assert(seg.sidedef->sector);
		assert(seg.frontsector);

		side_t& side = *seg.sidedef;
		if (side.toptexture == 0 && side.bottomtexture == 0 && side.midtexture == 0)
		{
			return;
		}

		if (seg.backsector)
		{
			sector_t& front = *seg.frontsector;
			sector_t& back = *seg.backsector;

			if (front.ceilingheight > back.ceilingheight && side.toptexture != 0)
			{
				AddSeg(seg, back.ceilingheight, front.ceilingheight);
			}

			if (side.midtexture != 0)
			{
				AddSeg(seg, max(front.floorheight, back.floorheight), min(front.ceilingheight, back.ceilingheight));
			}

			if (front.floorheight < back.floorheight && side.bottomtexture != 0)
			{
				AddSeg(seg, front.floorheight, back.floorheight);
			}
			return;
		}

		assert(side.toptexture == 0 && side.midtexture != 0 && side.bottomtexture == 0);

		const sector_t& sector = *side.sector;
		AddSeg(seg, sector.floorheight, sector.ceilingheight);
	}

	void UpdateRenderData(const vector<BspNodeInfo>& nodes, const vector<SegInfo>& segs) override
	{
		vector<bool> nodesActive(nodes.size(), !doingBspAnim);
		vector<bool> segsActive(segs.size(), !doingBspAnim);

		if (doingBspAnim)
		{
			if (nodes.size() && segs.size())
			{
				int curNode = 0;
				uint curSeg = 0;
				int stepsLeft = bspStep + 1;

				if (StepBsp(curNode, curSeg, stepsLeft, nodes, segs, nodesActive, segsActive) && !finishedBspAnim)
				{
					finishedBspAnim = true;
				}
			}
		}

		for (size_t i = 0; i < segs.size(); i++)
		{
			if (segsActive[i])
			{
				OnSeg(segs[i].seg);
			}
		}

		for (size_t i = 0; i < nodes.size(); i++)
		{
			OnBspNode(nodes[i], nodes);
		}

		for (int i = nodesActive.size() - 1; i >= 0; i--)
		{
			if (!nodesActive[i])
			{
				frameDataBuffer->nodes[i] = frameDataBuffer->nodes.back();
				frameDataBuffer->nodes.pop_back();
			}
		}
	}

	void OnBspNode(const BspNodeInfo& info, const vector<BspNodeInfo>& nodes)
	{
		assert(frameDataBuffer);

		frameDataBuffer->nodes.emplace_back();
		DoomBspNode& newNode = frameDataBuffer->nodes.back();
		newNode.twoSide = info.twoSide;

		XMVECTOR v1 = XMVectorSet(AppDefs::FixedToFloat(info.node.x), AppDefs::FixedToFloat(info.node.y), 0.0f, 0.0f);
		XMVECTOR delta = XMVectorSet(AppDefs::FixedToFloat(info.node.dx), AppDefs::FixedToFloat(info.node.dy), 0.0f, 0.0f);
		XMVECTOR expand = XMVectorScale(XMVector2Normalize(delta), 1000.0f);

		XMVECTOR sizedV1 = XMVectorSubtract(v1, expand);
		XMVECTOR sizedV2 = XMVectorAdd(XMVectorAdd(v1, delta), expand);

		int parent = info.parent;
		while (parent != AppDefs::NULL_NODE)
		{
			const DoomBspNode& parentNode = frameDataBuffer->nodes[parent];
			parent = nodes[parent].parent;

			XMVECTOR intersect = XMVector2IntersectLine(sizedV1, sizedV2, XMLoadFloat2(&parentNode.v1), XMLoadFloat2(&parentNode.v2));
			if (!XMVector2IsNaN(intersect) && !XMVector2IsInfinite(intersect))
			{
				if (!BoundPlane(sizedV1, intersect, v1))
				{
					BoundPlane(sizedV2, intersect, v1);
				}
			}
		}

		XMStoreFloat2(&newNode.v1, sizedV1);
		XMStoreFloat2(&newNode.v2, sizedV2);
	}

	bool BoundPlane(XMVECTOR& expanded, const XMVECTOR& intersect, const XMVECTOR& orig)
	{
		XMVECTOR toExpanded = XMVectorSubtract(expanded, intersect);
		XMVECTOR toOrig = XMVectorSubtract(orig, intersect);

		if (XMVector2LessOrEqual(XMVector2Dot(toExpanded, toOrig), g_XMZero))
		{
			expanded = intersect;
			return true;
		}
		return false;
	}

	void InitSound(int numChannels) override
	{
		audio.InitChannels(numChannels);
	}
	
	void PlaySound(const vector<ubyte>& samples, int sampleRate, int channel, int vol, int sep, int pitch) override
	{
		audio.PlaySound(samples, sampleRate, channel, vol, sep, pitch);
	}
	
	void UpdateSound(int channel, int vol, int sep) override
	{
		audio.UpdateSound(channel, vol, sep);
	}
	
	void StopSound(int channel) override
	{
		audio.StopSound(channel);
	}
	
	void PlaySong(const ubyte* data, int length, bool loop) override
	{
		audio.PlaySong(data, length, loop);
	}
	
	void StopSong() override
	{
		audio.StopSong();
	}
	
	void SetMusicVolume(int volume) override
	{
		audio.SetMusicVolume(volume);
	}

	Audio& audio;
	FrameType frameType;
	FrameDataWriter& frameData;
	FrameDataBuffer* frameDataBuffer;
	InputClient& input;
	array<ColorRgb, PALETTE_SIZE> palette;
	float camDist;
	float camYaw;
	float camPitch;
	int bspStep;
	int bspAnimTicks;
	bool finishedBspAnim;
	bool doingBspAnim;
	atomic_bool gameEnded;
	Thread gameThread;
};
