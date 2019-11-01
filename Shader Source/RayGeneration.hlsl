// RayGeneration.hlsl
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "CommonShaderStuff.h"

// Ray Generation shader, to begin the Raytracing flow.
[shader("raygeneration")]
void RayGeneration()
{
	// Collect the Thread Dimensions.
	uint2 ThreadDims = GetThreadDimensions();

	// Collect Thread Id for the current worker.
	uint2 ThreadId = GetThreadId();

	// Get the Camera's position in World-Space.
	float3 WorldCameraPosition = GetWorldCameraPosition(Constants.CameraToWorld);

	// Color value for the current pixel, to be accumulated during the loop, then averaged down afterwards and written to the Render Target.
	float3 PixelColor = {0.0, 0.0, 0.0};

	// Loop for creating and tracing rays within the current pixel (Pixel = Worker Thread).
	for (uint RayIndex = 1; RayIndex <= Constants.RaysPerPixel; RayIndex++)
	{
		// Collect the pixel offset values for the current ray.
		float2 PixelOffset = GetPixelOffset(RayIndex);

		// Collect the point in World-Space to shoot the current camera-ray through.
		float3 WorldPointPosition = GetWorldPointPosition(Constants.CameraToWorld, ThreadDims, ThreadId, PixelOffset, Constants.VertFoVRad);

		// Create and initialize the Ray Payload and Ray Description.		
		RayDesc CameraRay;
		CameraRay.Direction = normalize(WorldPointPosition - WorldCameraPosition);
		CameraRay.Origin = WorldCameraPosition;
		CameraRay.TMin = 0.000;
		CameraRay.TMax = 10000;

		RayPayload Payload;
		Payload.Color.x = 0.0;
		Payload.Color.y = 0.0;
		Payload.Color.z = 0.0;
		Payload.IntersectionCount = 0;
		Payload.RecursionDepth = 0;

		Payload.RecursionDepth++;

		TraceRay(Scene, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH, ~0, 0, 1, 0, CameraRay, Payload);

		if (Payload.IntersectionCount > 0)
		{
			Payload.Color = GetColorValue(Constants.SkyTopColor.xyz, Constants.SkyBottomColor.xyz, Payload.WorldLastScatterDirection);

			for (uint i = 1; i <= Payload.IntersectionCount; i++)
			{
				Payload.Color.x *= Constants.LambertianAttenuationValue;
				Payload.Color.y *= Constants.LambertianAttenuationValue;
				Payload.Color.z *= Constants.LambertianAttenuationValue;
			}
		}
		
		// Add the returned Ray's color value to the pixel's color value, to be averaged after.
		PixelColor.x += Payload.Color.x;
		PixelColor.y += Payload.Color.y;
		PixelColor.z += Payload.Color.z;
	}

	// Average the pixel's color value.
	PixelColor.x /= (float)Constants.RaysPerPixel;
	PixelColor.y /= (float)Constants.RaysPerPixel;
	PixelColor.z /= (float)Constants.RaysPerPixel;

	// Write the pixel's color value out to the Render Target.
	RenderTarget[ThreadId] = float4(PixelColor.x, PixelColor.y, PixelColor.z, 0.0);
}
