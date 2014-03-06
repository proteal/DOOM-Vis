//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

struct AppDefs
{
	static const int NULL_NODE = -1;
	static const int TICKS_PER_BSP_STEP = 7;
	static const int BSP_ANIM_HOLD_TICKS = 100;

	static float DEV_VIEW_MOVE_SPEED()
	{
		return 10.0f;
	}

	static float DEV_VIEW_TURN_SPEED()
	{
		return 0.001f;
	}

	static float FixedToFloat(const fixed_t& fixed)
	{
		return static_cast<float>(fixed) / FRACUNIT;
	}

	static void VertToFloat2(const vertex_t& vert, XMFLOAT2& float2)
	{
		float2.x = FixedToFloat(vert.x);
		float2.y = FixedToFloat(vert.y);
	}

	static float AngleToRadians(const angle_t& angle)
	{
		return XM_2PI * angle / UINT_MAX;
	}
};
