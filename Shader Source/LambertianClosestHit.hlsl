// LambertianClosestHit.hlsl
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "CommonShaderStuff.h"

// Lambertian Closest-Hit shader.
[shader("closesthit")]
void LambertianClosestHit(inout RayPayload Payload, in IntersectionAttributes Attributes)
{
	// Set some stuff in the Payload.
	Payload.IntersectionCount++;


	// Collect a random point in the Unit Sphere, for creating a random reflection/scatter direction.

	float3 WorldIntersectionPoint = mul(ObjectToWorld3x4(), float4(Attributes.ObjectIntersectionPoint, 1.0)).xyz;

	float3 WorldSurfaceNormal = normalize(mul(ObjectToWorld3x4(), float4(Attributes.ObjectSurfaceNormal, 0.0)).xyz);

	float3 RandomPointInUnitSphere = GetRandomPointInUnitSphere(Payload.IntersectionCount);

	float3 WorldScatterTarget = WorldIntersectionPoint + WorldSurfaceNormal + RandomPointInUnitSphere;

	Payload.WorldLastScatterDirection = normalize(WorldScatterTarget - WorldIntersectionPoint);

	if (Payload.RecursionDepth < Constants.MaxRecursionDepth)
	{
		// Create the Reflection Ray.
		RayDesc ReflectionRay;

		ReflectionRay.Origin = WorldIntersectionPoint;
		ReflectionRay.Direction = Payload.WorldLastScatterDirection;
		ReflectionRay.TMin = 0.0;
		ReflectionRay.TMax = 10000;
		
		Payload.RecursionDepth++;
		
		TraceRay(Scene, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH, ~InstanceID(), 0, 1, 0, ReflectionRay, Payload);
	}
}
