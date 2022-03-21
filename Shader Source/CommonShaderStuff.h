// CommonShaderStuff.h
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#define UnitSphereRadius 1.0

// Acceleration structure against which to trace rays.
RaytracingAccelerationStructure Scene : register(t0, space0);

// Render target of type UAV RW2DTexture.
RWTexture2D<unorm float4> RenderTarget : register(u0);

// Random Number Buffer of type UAV.
RWBuffer<float> RandomNumberBuffer : register(u1);

// Global "Constant Buffer" structure to pass all of the individual constants together.
struct InlineConstantBuffer
{
	// Camera-to-World transform, for moving the camera around in world-space.
	float4x4 CameraToWorld;

	// "Sky Color" at the top of the sky.
	float4 SkyTopColor;

	// "Sky Color" at the bottom of the sky.
	float4 SkyBottomColor;

	// Number of Random Floats.
	uint RandomFloatCount;
	float3 Pad01;

	// Vertical Field of View, in Radians.
	float VertFoVRad;

	// Max Recursion Depth.
	uint MaxRecursionDepth;

	// Number of Rays per pixel.
	uint RaysPerPixel;
	   	
	// For Lambertian Light Attenuation.
	float LambertianAttenuationValue;
};

ConstantBuffer<InlineConstantBuffer> Constants : register(b0);

// Ray payload for the main/only rays.
struct RayPayload
{
	// Final color "collected" by this Ray, to be calculated in the Miss and/or Closest Hit Shader.
	float3 Color;

	// Last scattering direction in World-Space.
	float3 WorldLastScatterDirection;

	// Tracker for number of times this ray has intersected something.
	uint IntersectionCount;
	
	// Tracker for number of times this ray has been traced, for limiting recursion depth. Increment this before each TraceRay() call.
	uint RecursionDepth;
};

// Intersection attributes.
struct IntersectionAttributes
{
	// Intersection point in Object-Space.
	float3 ObjectIntersectionPoint;

	// Surface normal of the intersection point in Object-Space.
	float3 ObjectSurfaceNormal;
};

// Enumeration for reporting the Hit-Type with ReportHit().
enum HitTypes
{
	SphereHit = 0
};

// Collects the Thread dimensions, as defined while making the DispatchRays() call.
uint2 GetThreadDimensions()
{
	uint2 ThreadDims = {DispatchRaysDimensions().x, DispatchRaysDimensions().y};
	return ThreadDims;
}

// Collects the current worker's Thread ID.
uint2 GetThreadId()
{
	uint2 ThreadId = {DispatchRaysIndex().x, DispatchRaysIndex().y};
	return ThreadId;
}

// Calculates the Camera's position in World-Space, using a Camera-to-World transform.
float3 GetWorldCameraPosition(float4x4 CameraToWorld)
{
	// The second value represents the Camera's position in the Camera's Object-Space, along with a homogenous coordinate for translation.
	return mul(CameraToWorld, float4(0.0, 0.0, 0.0, 1.0)).xyz;
}

// Function for wrapping requested index values into the target resource's available range of elements.
uint GetWrappedIndex(uint RequestedIndex, uint TotalElements)
{
	uint WrappedIndex = (uint)((uint)RequestedIndex % (uint)TotalElements);

	return WrappedIndex;
}

// Calculates random offsets into a pixel (Range [0.0, 1.0]), for jittering ray positions. Uses the Wang Hash function.
float2 GetPixelOffset(uint CurrentRayWithinPixel)
{
	// Generate valid indices for collecting random floats.
	uint RequestedIndex_x = (CurrentRayWithinPixel - 1) * 2 + 0;
	uint RequestedIndex_y = (CurrentRayWithinPixel - 1) * 2 + 1;

	uint Index_x = GetWrappedIndex(RequestedIndex_x, Constants.RandomFloatCount);
	uint Index_y = GetWrappedIndex(RequestedIndex_y, Constants.RandomFloatCount);

	// Generate the pixel offsets.
	float2 PixelOffset = {0.0, 0.0};
	
	PixelOffset.x = 0.5 * RandomNumberBuffer[Index_x] + 0.5;
	PixelOffset.y = 0.5 * RandomNumberBuffer[Index_y] + 0.5;
	
	return PixelOffset;
}

// Calculates a Point's position in World-Space, using a Camera-to-World transform and some other values. Used for generating Camera-Ray directions.
float3 GetWorldPointPosition(float4x4 CameraToWorld, uint2 ThreadDims, uint2 ThreadId, float2 OffsetIntoPixel, float VerticalFoVRadians)
{
	float3 WorldPointPosition;
	
	WorldPointPosition.x = ((((float)ThreadId.x + OffsetIntoPixel.x) / (float)ThreadDims.x) * 2.0 - 1.0) * ((float)ThreadDims.x / (float)ThreadDims.y) * (tan(VerticalFoVRadians / 2.0));

	WorldPointPosition.y = -((((float)ThreadId.y + OffsetIntoPixel.y) / (float)ThreadDims.y) * 2.0 - 1.0) * (tan(VerticalFoVRadians / 2.0));

	WorldPointPosition.z = 1.0;
	
	return mul(CameraToWorld, float4(WorldPointPosition, 1.0)).xyz;
}

// Function for "lerping" the "sky-color" associated with a given pixel, based on World-Space Ray Direction.
float3 GetColorValue(float3 SkyTopColor, float3 SkyBottomColor, float3 WorldRayDirection)
{
	float3 ColorValue = {0.0, 0.0, 0.0};
	
	ColorValue.x = ((1.0 - (0.5 * WorldRayDirection.y + 0.5)) * SkyBottomColor.x) + ((0.5 * WorldRayDirection.y + 0.5) * SkyTopColor.x);
	ColorValue.y = ((1.0 - (0.5 * WorldRayDirection.y + 0.5)) * SkyBottomColor.y) + ((0.5 * WorldRayDirection.y + 0.5) * SkyTopColor.y);
	ColorValue.z = ((1.0 - (0.5 * WorldRayDirection.y + 0.5)) * SkyBottomColor.z) + ((0.5 * WorldRayDirection.y + 0.5) * SkyTopColor.z);
	
	return ColorValue;
}

// Function for checking the number of solutions to the Sphere's "Intersection Quadratic".
float GetIntersectionCount()
{
	// 'a' from the quadratic formula.
	float a = dot(ObjectRayDirection(), ObjectRayDirection());
	
	// 'b' from the quadratic formula.
	float3 ObjectSphereCenter = {0.0, 0.0, 0.0};
	float b = 2.0 * dot(ObjectRayDirection(), (ObjectRayOrigin() - ObjectSphereCenter));

	// 'c' from the quadratic formula.
	float c = dot((ObjectRayOrigin() - ObjectSphereCenter), (ObjectRayOrigin() - ObjectSphereCenter)) - (UnitSphereRadius * UnitSphereRadius);

	float Discriminant = (b * b) - (4 * a * c);

	return Discriminant;
}

// Function for calculating the position of the Sphere intersection, in World-Space.
float3 GetWorldIntersectionPoint()
{
	// 'a' from the quadratic formula.
	float a = dot(ObjectRayDirection(), ObjectRayDirection());
	
	// 'b' from the quadratic formula.
	float3 ObjectSphereCenter = {0.0, 0.0, 0.0};
	float b = 2.0 * dot(ObjectRayDirection(), (ObjectRayOrigin() - ObjectSphereCenter));

	// 'c' from the quadratic formula.
	float c = dot((ObjectRayOrigin() - ObjectSphereCenter), (ObjectRayOrigin() - ObjectSphereCenter)) - (UnitSphereRadius * UnitSphereRadius);

	float Discriminant = (b * b) - (4 * a * c);
	
	// Distance along Ray to the intersection.
	float tRay = (-b + sqrt(Discriminant)) / (2.0 * a);
	
	// Sphere intersection point, in Object-Space.
	float3 ObjectIntersectionPoint;

	ObjectIntersectionPoint = (ObjectRayOrigin() + (tRay * ObjectRayDirection())) / UnitSphereRadius;
	
	// Sphere intersection point, in World-Space.
	float3 WorldIntersectionPoint;

	WorldIntersectionPoint = mul(ObjectToWorld3x4(), float4(ObjectIntersectionPoint, 1.0)).xyz;
	
	return WorldIntersectionPoint;
}

// Function for calculating the position of the Sphere intersection, in Object-Space.
float3 GetObjectIntersectionPoint()
{
	// 'a' from the quadratic formula.
	float a = dot(ObjectRayDirection(), ObjectRayDirection());
	
	// 'b' from the quadratic formula.
	float3 ObjectSphereCenter = {0.0, 0.0, 0.0};
	float b = 2.0 * dot(ObjectRayDirection(), (ObjectRayOrigin() - ObjectSphereCenter));

	// 'c' from the quadratic formula.
	float c = dot((ObjectRayOrigin() - ObjectSphereCenter), (ObjectRayOrigin() - ObjectSphereCenter)) - (UnitSphereRadius * UnitSphereRadius);

	float Discriminant = (b * b) - (4 * a * c);
	
	// Distance along Ray to the intersection.
	float tRay = (-b - sqrt(Discriminant)) / (2.0 * a);
	
	// Sphere intersection point, in Object-Space.
	float3 ObjectIntersectionPoint;

	ObjectIntersectionPoint = (ObjectRayOrigin() + (tRay * ObjectRayDirection())) / UnitSphereRadius;
		
	return ObjectIntersectionPoint;
}

// Function for calculating the Surface Normal of the Sphere intersection, in World-Space.
float3 GetWorldSurfaceNormal(float3 WorldIntersectionPoint)
{
	float3 ObjectIntersectionPoint = mul(WorldToObject3x4(), float4(WorldIntersectionPoint, 1.0)).xyz;

	float3 WorldSurfaceNormal = normalize(mul(ObjectToWorld3x4(), float4(ObjectIntersectionPoint, 0.0)).xyz);
	
	return WorldSurfaceNormal;
}

// Function for calculating the Surface Normal of the Sphere intersection, in Object-Space.
float3 GetObjectSurfaceNormal(float3 ObjectIntersectionPoint)
{	
	float3 ObjectSurfaceNormal = normalize(ObjectIntersectionPoint);

	return ObjectSurfaceNormal;
}

// Function for calculating a "random" point inside a Unit Sphere'.
float3 GetRandomPointInUnitSphere(uint IntersectionCount)
{
	float3 RandomPointInUnitSphere;

	// Collect valid indices.
	uint RequestedIndex_x = ((DispatchRaysIndex().y * DispatchRaysDimensions().x + DispatchRaysIndex().x) * 3) + IntersectionCount + 0;
	uint RequestedIndex_y = ((DispatchRaysIndex().y * DispatchRaysDimensions().x + DispatchRaysIndex().x) * 3) + IntersectionCount + 1;
	uint RequestedIndex_z = ((DispatchRaysIndex().y * DispatchRaysDimensions().x + DispatchRaysIndex().x) * 3) + IntersectionCount + 2;

	uint Index_x = GetWrappedIndex(RequestedIndex_x, Constants.RandomFloatCount);
	uint Index_y = GetWrappedIndex(RequestedIndex_y, Constants.RandomFloatCount);
	uint Index_z = GetWrappedIndex(RequestedIndex_z, Constants.RandomFloatCount);

	RandomPointInUnitSphere.x = RandomNumberBuffer[Index_x];
	RandomPointInUnitSphere.y = RandomNumberBuffer[Index_y];
	RandomPointInUnitSphere.z = RandomNumberBuffer[Index_z];

	return RandomPointInUnitSphere;
}
