// MetallicMiss.hlsl
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "CommonShaderStuff.h"

// Metallic Miss shader.
[shader("miss")]
void MetallicMiss(inout RayPayload Payload)
{
	if (Payload.IntersectionCount == 0)
	{
		Payload.Color = GetColorValue(Constants.SkyTopColor.xyz, Constants.SkyBottomColor.xyz, WorldRayDirection());
	}
}
