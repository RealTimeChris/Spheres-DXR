// MetallicIntersection.hlsl
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "CommonShaderStuff.h"

// Metallic Intersection shader.
[shader("intersection")]
void MetallicIntersection()
{
	// Figure out if we actually made any intersections with the AABB's sphere.
	float Discriminant = GetIntersectionCount();

	// Report the intersection, if there are 1 or 2 solutions to the quadratic/intersections with the sphere.
	if (Discriminant >= 0.0)
	{
		// For reporting the Sphere Intersection Attributes.
		IntersectionAttributes Attributes;
		
		// Collect the intersection point, in Object-Space.
		Attributes.ObjectIntersectionPoint = GetObjectIntersectionPoint();

		// Collect the Surface Normal, in Object-Space.
		Attributes.ObjectSurfaceNormal = GetObjectSurfaceNormal(Attributes.ObjectIntersectionPoint);

		// Report the hit.
		ReportHit(RayTCurrent(), SphereHit, Attributes);
	}
}
