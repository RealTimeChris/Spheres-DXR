// MetallicClosestHit.hlsl
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "CommonShaderStuff.h"

// Metallic Closest-Hit shader.
[shader("closesthit")]
void MetallicClosestHit(inout RayPayload Payload, in IntersectionAttributes Attributes)
{
	// Set some stuff in the Payload.
	Payload.IntersectionCount++;
	
}
