// Main.cpp.cpp - Main source file for Spheres (DXR).
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#if _DEBUG
#define DebugSwitch true
#else
#define DebugSwitch false
#endif

#ifndef _CRT_RAND_S
#define _CRT_RAND_S
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <DirectXMath.h>
#include <string>

#include "Win32Window.hpp"
#include "WD3D12.hpp"
#include "WDXGI.hpp"
#include "RGBAWelcomeMat.hpp"

#include "Compiled Shaders/DielectricAnyHit.h"
#include "Compiled Shaders/DielectricIntersection.h"
#include "Compiled Shaders/DielectricMiss.h"
#include "Compiled Shaders/LambertianClosestHit.h"
#include "Compiled Shaders/LambertianIntersection.h"
#include "Compiled Shaders/LambertianMiss.h"
#include "Compiled Shaders/MetallicClosestHit.h"
#include "Compiled Shaders/MetallicIntersection.h"
#include "Compiled Shaders/MetallicMiss.h"
#include "Compiled Shaders/RayGeneration.h"

// Sphere center as a set of 3D coordinates.
struct SphereCenter
{
	float x, y, z;
};

// Returns an argument-defined D3D12 Resource Barrier (Transiton type).
inline D3D12_RESOURCE_BARRIER CreateResourceTransitionBarrier(
	ID3D12Resource* pResource_v0,
	UINT SubResource,
	D3D12_RESOURCE_STATES StateBefore,
	D3D12_RESOURCE_STATES StateAfter,
	D3D12_RESOURCE_BARRIER_FLAGS ResourceBarrierFlags = D3D12_RESOURCE_BARRIER_FLAG_NONE
)
{
	D3D12_RESOURCE_BARRIER ResourceBarrier{};
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Flags = ResourceBarrierFlags;
	ResourceBarrier.Transition.pResource = pResource_v0;
	ResourceBarrier.Transition.Subresource = SubResource;
	ResourceBarrier.Transition.StateBefore = StateBefore;
	ResourceBarrier.Transition.StateAfter = StateAfter;

	return ResourceBarrier;
}

// Flushes a given Command Queue by signalling a Fence to fire an event, and then waiting for the event.
inline void FlushCommandQueue(
	ID3D12CommandQueue* pCommandQueue_v0,
	ID3D12Fence* pFence_v0,
	unsigned __int64* pFenceValue,
	unsigned __int64 FenceIncrement
)
{
	HANDLE FenceEvent{};

	pCommandQueue_v0->Signal(pFence_v0, *pFenceValue + FenceIncrement);

	pFence_v0->SetEventOnCompletion(*pFenceValue + FenceIncrement, FenceEvent);

	WaitForSingleObject(FenceEvent, INFINITE);

	*pFenceValue = pFence_v0->GetCompletedValue();
}

// Copies data from host memory into a shared GPU upload buffer.
inline void MemCopyToUploadBuffer(
	ID3D12Resource* pUploadResource_v0,
	rsize_t DestinationSize,
	void* pSourceData,
	rsize_t SourceSize,
	UINT UploadSubResource = 0
)
{
	HRESULT Result{ 1 };

	void* MappedMemory{ nullptr };

	WD3D12::FailCheck(
		pUploadResource_v0->Map(UploadSubResource, nullptr, &MappedMemory),
		L"ID3D12Resource.Map() failed.",
		L"MemCopyToUploadBuffer() error."
	);

	Result = memcpy_s(MappedMemory, DestinationSize, pSourceData, SourceSize);

	if (Result != 0)
	{
		MessageBoxW(NULL, L"memcpy_s() failed.", L"MemCopyToUploadBuffer() error.", NULL);
	}

	pUploadResource_v0->Unmap(UploadSubResource, nullptr);

	MappedMemory = nullptr;
}

// Collects a CPU and GPU descriptor handle to a given slot within a descriptor heap.
inline void GetDescriptorHandles(
	ID3D12DescriptorHeap* pDescriptorHeap,
	unsigned __int64 DescriptorHeapOffset,
	UINT DescriptorHandleIncrementSize,
	D3D12_CPU_DESCRIPTOR_HANDLE* pCPUDescriptorHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE* pGPUDescriptorHandle
)
{
	pCPUDescriptorHandle->ptr =
		pDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + (DescriptorHeapOffset * DescriptorHandleIncrementSize);

	pGPUDescriptorHandle->ptr =
		pDescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + (DescriptorHeapOffset * DescriptorHandleIncrementSize);
}





int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	// Some general application values/parameters.

	// Number of pixels, and bytes-per-pixel, for the desired rendering format.
	const unsigned __int32 PixelWidth{ 3840U };
	const unsigned __int32 PixelHeight{ 2160U };
	const unsigned __int32 PixelCount{ PixelWidth * PixelHeight };
	const unsigned __int32 BytesPerPixel{ 4U };

	// Byte denominations.
	const unsigned __int32 BytesPerKilobyte{ 1024U };
	const unsigned __int32 KilobytesPerMegabyte{ 1024U };
	const unsigned __int32 MegabytesPerGigabyte{ 1024U };
	

	
	

	// Debug and Device interfaces.
	using namespace WD3D12;

	// D3D12 debug interface.
	WD3D12Debug0 D3D12Debug{};
	D3D12Debug.InitConfig.enable_debug_layer = DebugSwitch;
	D3D12Debug.Initialize();

	// D3D12 device interface.
	WD3D12Device5 Device{};
	Device.InitConfig.unicode_debug_name = L"Device";
	Device.Initialize();

	// Direct command queue, for submitting/executing GPU work.
	WD3D12CommandQueue0 DirectCommandQueue{};
	DirectCommandQueue.InitConfig.unicode_debug_name = L"DirectCommandQueue";
	DirectCommandQueue.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	DirectCommandQueue.InitConfig.d3d12_command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	DirectCommandQueue.InitConfig.d3d12_command_queue_desc.NodeMask = 0;
	DirectCommandQueue.InitConfig.d3d12_command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	DirectCommandQueue.InitConfig.d3d12_command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	DirectCommandQueue.Initialize();

	// Command allocator, for managing command-related memory on the GPU.
	WD3D12CommandAllocator0 DirectCommandAllocator{};
	DirectCommandAllocator.InitConfig.unicode_debug_name = L"DirectCommandAllocator";
	DirectCommandAllocator.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	DirectCommandAllocator.InitConfig.d3d12_command_list_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	DirectCommandAllocator.Initialize();

	// Fence and utility values, for synchronizing the CPU and GPU.
	unsigned __int64 FenceValue{ 0U };
	const unsigned __int64 FenceIncrement{ 1U };

	WD3D12Fence1 Fence{};
	Fence.InitConfig.unicode_debug_name = L"Fence";
	Fence.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	Fence.InitConfig.fence_initial_value = FenceValue;
	Fence.Initialize();

	// Upload heap.
	const unsigned __int64 UploadHeapByteSize{ (MegabytesPerGigabyte * KilobytesPerMegabyte * BytesPerKilobyte) / 8U };

	WD3D12Heap0 UploadHeap{};
	UploadHeap.InitConfig.unicode_debug_name = L"UploadHeap";
	UploadHeap.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	UploadHeap.InitConfig.d3d12_heap_description.SizeInBytes = UploadHeapByteSize;
	UploadHeap.InitConfig.d3d12_heap_description.Flags = D3D12_HEAP_FLAG_NONE;
	UploadHeap.InitConfig.d3d12_heap_description.Properties.Type = D3D12_HEAP_TYPE_CUSTOM;
	UploadHeap.InitConfig.d3d12_heap_description.Properties.VisibleNodeMask = 0;
	UploadHeap.InitConfig.d3d12_heap_description.Properties.CreationNodeMask = 0;
	UploadHeap.InitConfig.d3d12_heap_description.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	UploadHeap.InitConfig.d3d12_heap_description.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	UploadHeap.Initialize();

	// Heap of dedicated GPU memory.
	const unsigned __int64 PipelineHeapByteSize{ (MegabytesPerGigabyte * KilobytesPerMegabyte * BytesPerKilobyte) / 2U };

	WD3D12Heap0 PipelineHeap{};
	PipelineHeap.InitConfig.unicode_debug_name = L"PipelineHeap";
	PipelineHeap.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	PipelineHeap.InitConfig.d3d12_heap_description.Flags = D3D12_HEAP_FLAG_NONE;
	PipelineHeap.InitConfig.d3d12_heap_description.SizeInBytes = PipelineHeapByteSize;
	PipelineHeap.InitConfig.d3d12_heap_description.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
	PipelineHeap.InitConfig.d3d12_heap_description.Properties.CreationNodeMask = 0;
	PipelineHeap.InitConfig.d3d12_heap_description.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;
	PipelineHeap.InitConfig.d3d12_heap_description.Properties.Type = D3D12_HEAP_TYPE_CUSTOM;
	PipelineHeap.InitConfig.d3d12_heap_description.Properties.VisibleNodeMask = 0;
	PipelineHeap.Initialize();

	// Descriptor Heap for CBVs, SRVs, and UAVs.
	UINT DescriptorHandleIncrementSize
	{
		Device.GetInterface()->GetDescriptorHandleIncrementSize
		(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		)
	};

	unsigned __int64 DescriptorHeapOffset{ 0 };

	WD3D12DescriptorHeap0 DescriptorHeap{};
	DescriptorHeap.InitConfig.unicode_debug_name = L"DescriptorHeap";
	DescriptorHeap.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	DescriptorHeap.InitConfig.id3d12_descriptor_heap_description.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DescriptorHeap.InitConfig.id3d12_descriptor_heap_description.NodeMask = 0;
	DescriptorHeap.InitConfig.id3d12_descriptor_heap_description.NumDescriptors = 3;
	DescriptorHeap.InitConfig.id3d12_descriptor_heap_description.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DescriptorHeap.Initialize();





	// Create and initialize the IntersectionMap with RGBA data.

	// Resources, Texture Copy Locations and Resource Barriers for the Intersection Map.
	const unsigned __int32 IntersectionMapResourceByteSize{ PixelCount * BytesPerPixel };

	WD3D12PlacedResource0 IntersectionMapUploadResource{};
	IntersectionMapUploadResource.InitConfig.unicode_debug_name = L"IntersectionMapUploadResource";
	IntersectionMapUploadResource.InitConfig.ptr_id3d12heap_v0 = UploadHeap.GetInterface();
	IntersectionMapUploadResource.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	IntersectionMapUploadResource.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	IntersectionMapUploadResource.InitConfig.heap_offset_in_bytes = (2 * 65536);
	IntersectionMapUploadResource.InitConfig.d3d12_resource_description.Width = IntersectionMapResourceByteSize;
	IntersectionMapUploadResource.Initialize();

	D3D12_TEXTURE_COPY_LOCATION TextureCopyLocationIntersectionMapUploadResource{};
	TextureCopyLocationIntersectionMapUploadResource.pResource = IntersectionMapUploadResource.GetInterface();
	TextureCopyLocationIntersectionMapUploadResource.SubresourceIndex = 0;
	TextureCopyLocationIntersectionMapUploadResource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	TextureCopyLocationIntersectionMapUploadResource.PlacedFootprint.Offset = 0;
	TextureCopyLocationIntersectionMapUploadResource.PlacedFootprint.Footprint.Width = PixelWidth;
	TextureCopyLocationIntersectionMapUploadResource.PlacedFootprint.Footprint.Height = PixelHeight;
	TextureCopyLocationIntersectionMapUploadResource.PlacedFootprint.Footprint.Depth = 1;
	TextureCopyLocationIntersectionMapUploadResource.PlacedFootprint.Footprint.RowPitch = PixelWidth * BytesPerPixel;
	TextureCopyLocationIntersectionMapUploadResource.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;

	WD3D12PlacedResource0 IntersectionMap2DTexture{};
	IntersectionMap2DTexture.InitConfig.unicode_debug_name = L"IntersectionMap2DTexture";
	IntersectionMap2DTexture.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	IntersectionMap2DTexture.InitConfig.ptr_id3d12heap_v0 = PipelineHeap.GetInterface();
	IntersectionMap2DTexture.InitConfig.heap_offset_in_bytes = 6 * 65536;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.Width = PixelWidth;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.Height = PixelHeight;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.DepthOrArraySize = 1;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.MipLevels = 1;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.SampleDesc.Count = 1;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.SampleDesc.Quality = 0;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	IntersectionMap2DTexture.InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	IntersectionMap2DTexture.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_COPY_DEST;
	IntersectionMap2DTexture.Initialize();

	// Create the Unordered Access View of the Intersection Map to be used for binding it to the pipeline, and giving it shader access.
	D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDescription_IntersectionMap{};
	UAVDescription_IntersectionMap.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UAVDescription_IntersectionMap.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	UAVDescription_IntersectionMap.Texture2D.MipSlice = 0;
	UAVDescription_IntersectionMap.Texture2D.PlaneSlice = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandleToIntersectionMap2DTextureUAV{};
	D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandleToIntersectionMap2DTextureUAV{};

	GetDescriptorHandles
	(
		DescriptorHeap.GetInterface(),
		DescriptorHeapOffset,
		DescriptorHandleIncrementSize,
		&CPUDescriptorHandleToIntersectionMap2DTextureUAV,
		&GPUDescriptorHandleToIntersectionMap2DTextureUAV
	);

	DescriptorHeapOffset++;

	Device.GetInterface()->CreateUnorderedAccessView
	(
		IntersectionMap2DTexture.GetInterface(),
		nullptr,
		&UAVDescription_IntersectionMap,
		CPUDescriptorHandleToIntersectionMap2DTextureUAV
	);

	D3D12_TEXTURE_COPY_LOCATION TextureCopyLocationIntersectionMap2DTexture{};
	TextureCopyLocationIntersectionMap2DTexture.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	TextureCopyLocationIntersectionMap2DTexture.pResource = IntersectionMap2DTexture.GetInterface();
	TextureCopyLocationIntersectionMap2DTexture.SubresourceIndex = 0;

	// Generate the RGBA data for the intersection map and move it into the upload buffer.
	RGBAWelcomeMat WelcomeMat{};
	WelcomeMat.InitConfig.color_format = COLOR_FORMAT_R8G8B8A8;
	WelcomeMat.InitConfig.pixel_width = PixelWidth;
	WelcomeMat.InitConfig.pixel_height = PixelHeight;
	WelcomeMat.Initialize();

	void* CPUPointerToIntersectionMapUploadBuffer{};

	WD3D12::FailCheck
	(
		IntersectionMapUploadResource.GetInterface()->Map
		(
			0,
			nullptr,
			(void**)& CPUPointerToIntersectionMapUploadBuffer
		),
		L"Mapping upload buffer failed",
		L"Mapping upload buffer failed"
	);

	WelcomeMat.Greet
	(
		CPUPointerToIntersectionMapUploadBuffer
	);

	IntersectionMapUploadResource.GetInterface()->Unmap
	(
		0,
		nullptr
	);
	CPUPointerToIntersectionMapUploadBuffer = nullptr;





	// Create a buffer filled with random numbers, for generating random direction vectors in the shaders.
	const unsigned __int64 RandomFloatCount{ PixelCount };
	const unsigned __int64 RandomNumberUploadBufferByteSize{ RandomFloatCount * sizeof(float) };

	// Upload buffer with CPU and GPU access.
	WD3D12CommittedResource0 RandomNumberUploadBuffer{};
	RandomNumberUploadBuffer.InitConfig.unicode_debug_name = L"RandomNumberUploadBuffer";
	RandomNumberUploadBuffer.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	RandomNumberUploadBuffer.InitConfig.d3d12_heap_properties.Type = D3D12_HEAP_TYPE_CUSTOM;
	RandomNumberUploadBuffer.InitConfig.d3d12_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	RandomNumberUploadBuffer.InitConfig.d3d12_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	RandomNumberUploadBuffer.InitConfig.d3d12_heap_properties.CreationNodeMask = 0;
	RandomNumberUploadBuffer.InitConfig.d3d12_heap_properties.VisibleNodeMask = 0;
	RandomNumberUploadBuffer.InitConfig.d3d12_heap_flags = D3D12_HEAP_FLAG_NONE;
	RandomNumberUploadBuffer.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	RandomNumberUploadBuffer.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	RandomNumberUploadBuffer.InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_NONE;
	RandomNumberUploadBuffer.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
	RandomNumberUploadBuffer.InitConfig.d3d12_resource_description.Width = RandomNumberUploadBufferByteSize;
	RandomNumberUploadBuffer.InitConfig.d3d12_resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	RandomNumberUploadBuffer.Initialize();

	// Generate the Pseudo-Random numbers (Range [-1.0, +1.0]) here on the host-side.
	float* pRandomNumbers{ nullptr };
	pRandomNumbers = new float[RandomFloatCount];

	int RandomNumber{ 0 };
	float RandomComponent{ 0.0f };

	for (unsigned __int32 i = 0; i < RandomFloatCount; i++)
	{
		RandomNumber = rand();
		RandomComponent = 2.0f * ((float)RandomNumber / (float)RAND_MAX) - 1.0f;
		pRandomNumbers[i] = RandomComponent;
	}

	// Copy the numbers into the upload buffer.
	MemCopyToUploadBuffer
	(
		RandomNumberUploadBuffer.GetInterface(),
		RandomNumberUploadBufferByteSize,
		pRandomNumbers,
		RandomNumberUploadBufferByteSize
	);

	delete[] pRandomNumbers;
	pRandomNumbers = nullptr;

	// GPU-Only random number buffer.
	WD3D12CommittedResource0 RandomNumberBuffer{};
	RandomNumberBuffer.InitConfig.unicode_debug_name = L"RandomNumberBuffer";
	RandomNumberBuffer.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	RandomNumberBuffer.InitConfig.d3d12_heap_properties.Type = D3D12_HEAP_TYPE_CUSTOM;
	RandomNumberBuffer.InitConfig.d3d12_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
	RandomNumberBuffer.InitConfig.d3d12_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;
	RandomNumberBuffer.InitConfig.d3d12_heap_properties.CreationNodeMask = 0;
	RandomNumberBuffer.InitConfig.d3d12_heap_properties.VisibleNodeMask = 0;
	RandomNumberBuffer.InitConfig.d3d12_heap_flags = D3D12_HEAP_FLAG_NONE;
	RandomNumberBuffer.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	RandomNumberBuffer.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	RandomNumberBuffer.InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	RandomNumberBuffer.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
	RandomNumberBuffer.InitConfig.d3d12_resource_description.Width = RandomNumberUploadBufferByteSize;
	RandomNumberBuffer.InitConfig.d3d12_resource_state = D3D12_RESOURCE_STATE_COPY_DEST;
	RandomNumberBuffer.Initialize();

	D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandleToRandomNumberBuffer{};
	D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandleToRandomNumberBuffer{};

	GetDescriptorHandles(
		DescriptorHeap.GetInterface(),
		DescriptorHeapOffset,
		DescriptorHandleIncrementSize,
		&CPUDescriptorHandleToRandomNumberBuffer,
		&GPUDescriptorHandleToRandomNumberBuffer
	);

	DescriptorHeapOffset++;

	D3D12_UNORDERED_ACCESS_VIEW_DESC RandomNumberBufferUAVDescription{};
	RandomNumberBufferUAVDescription.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	RandomNumberBufferUAVDescription.Format = DXGI_FORMAT_R32_TYPELESS;
	RandomNumberBufferUAVDescription.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
	RandomNumberBufferUAVDescription.Buffer.FirstElement = 0;
	RandomNumberBufferUAVDescription.Buffer.NumElements = RandomFloatCount;

	Device.GetInterface()->CreateUnorderedAccessView(
		RandomNumberBuffer.GetInterface(),
		nullptr,
		&RandomNumberBufferUAVDescription,
		CPUDescriptorHandleToRandomNumberBuffer
	);





	// Collection of scene and rendering constants, to be passed into the shaders as a collection of global inline root constants.
	using namespace DirectX;

	// Global "Constant Buffer" structure to contain scene data for the shaders.
	struct InlineConstantBuffer
	{
		// Camera-to-World transform, for moving the camera around in world-space.
		XMMATRIX CameraToWorld;

		// "Sky Color" at the top of the sky.
		XMFLOAT4 SkyTopColor;

		// "Sky Color" at the bottom of the sky.
		XMFLOAT4 SkyBottomColor;

		// Number of Random Floats.
		UINT RandomFloatCount;
		float Pad01[3];

		// Vertical Field of View, in Radians.
		float VertFoVRad;

		// Max Recursion Depth.
		UINT MaxRecursionDepth;

		// Number of Rays per pixel.
		UINT RaysPerPixel;

		// For Lambertian Light Attenuation.
		float LambertianAttenuationValue;
	};

	// Constant buffer of inline root constants.
	InlineConstantBuffer InlineConstantBuffer{};

	// Camera-to-World transform, for moving the camera around in world-space.
	XMFLOAT3 CameraPosition{ 0.0f, 20.0f, 30.0001f };
	XMFLOAT3 FocusPoint{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 UpDirection{ 0.0f, +1.0f, 0.0f };

	InlineConstantBuffer.CameraToWorld = XMMatrixInverse
	(
		nullptr,
		XMMatrixLookAtLH
		(
			XMLoadFloat3(&CameraPosition),
			XMLoadFloat3(&FocusPoint),
			XMLoadFloat3(&UpDirection)
		)
	);

	// "Sky Color" at the top of the sky.
	InlineConstantBuffer.SkyTopColor.x = 0.0f;
	InlineConstantBuffer.SkyTopColor.y = 0.502f;
	InlineConstantBuffer.SkyTopColor.z = 1.0f;
	InlineConstantBuffer.SkyTopColor.w = 0.0f;

	// "Sky Color" at the bottom of the sky.
	InlineConstantBuffer.SkyBottomColor.x = 1.0f;
	InlineConstantBuffer.SkyBottomColor.y = 1.0f;
	InlineConstantBuffer.SkyBottomColor.z = 1.0f;
	InlineConstantBuffer.SkyBottomColor.w = 0.0f;

	// Number of Random Floats.
	InlineConstantBuffer.RandomFloatCount = RandomFloatCount;

	// Vertical field-of-view in radians.
	InlineConstantBuffer.VertFoVRad = XMConvertToRadians(90.0f);

	// Max Recursion Depth.
	InlineConstantBuffer.MaxRecursionDepth = 31U;

	// Number of Rays per pixel.
	InlineConstantBuffer.RaysPerPixel = 500U;

	// For Lambertian Light Attenuation.
	InlineConstantBuffer.LambertianAttenuationValue = 0.5f;

	// Total number of 32-bit Inline Root Constants, for the Global Root Signature.
	const unsigned __int64 InlineConstantsCount{ sizeof(InlineConstantBuffer) / sizeof(__int32) };





	// Build the bottom level acceleration structure(s).

	// Axis-aligned bounding boxes for the unit-sphere.
	const SphereCenter UnitSphereCenter{ 0.0f, 0.0f, 0.0f };
	const float UnitSphereRadius{ +1.0 };

	D3D12_RAYTRACING_AABB UnitSphereAABB{};
	UnitSphereAABB.MinX = UnitSphereCenter.x - UnitSphereRadius;
	UnitSphereAABB.MinY = UnitSphereCenter.y - UnitSphereRadius;
	UnitSphereAABB.MinZ = UnitSphereCenter.z - UnitSphereRadius;
	UnitSphereAABB.MaxX = UnitSphereCenter.x + UnitSphereRadius;
	UnitSphereAABB.MaxY = UnitSphereCenter.y + UnitSphereRadius;
	UnitSphereAABB.MaxZ = UnitSphereCenter.z + UnitSphereRadius;

	// Stuff the AABB(s) into an array.
	const unsigned __int64 AABBCount{ 1 };

	D3D12_RAYTRACING_AABB AABBGeometryArray[AABBCount]{ UnitSphereAABB };

	// Upload resource for the AABBs.
	const unsigned __int64 AABBUploadResourceByteSize{ AABBCount * sizeof(D3D12_RAYTRACING_AABB) };

	WD3D12PlacedResource0 AABBUploadResource{};
	AABBUploadResource.InitConfig.unicode_debug_name = L"AABBUploadResource";
	AABBUploadResource.InitConfig.ptr_id3d12heap_v0 = UploadHeap.GetInterface();
	AABBUploadResource.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	AABBUploadResource.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	AABBUploadResource.InitConfig.heap_offset_in_bytes = 0;
	AABBUploadResource.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	AABBUploadResource.InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_NONE;
	AABBUploadResource.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
	AABBUploadResource.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	AABBUploadResource.InitConfig.d3d12_resource_description.Width = AABBUploadResourceByteSize;
	AABBUploadResource.Initialize();

	// Copy the AABB(s) into the upload resource.
	MemCopyToUploadBuffer
	(
		AABBUploadResource.GetInterface(),
		sizeof(AABBGeometryArray),
		AABBGeometryArray,
		sizeof(AABBGeometryArray)
	);

	// Dedicated GPU-Only resource for the AABBs, to be used for building the acceleration structures.
	const unsigned __int64 AABBPipelineResourceByteSize{ AABBUploadResourceByteSize };

	WD3D12PlacedResource0 AABBPipelineResource{};
	AABBPipelineResource.InitConfig.unicode_debug_name = L"AABBPipelineResource";
	AABBPipelineResource.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	AABBPipelineResource.InitConfig.ptr_id3d12heap_v0 = PipelineHeap.GetInterface();
	AABBPipelineResource.InitConfig.heap_offset_in_bytes = 0;
	AABBPipelineResource.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	AABBPipelineResource.InitConfig.d3d12_resource_description.Width = AABBPipelineResourceByteSize;
	AABBPipelineResource.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
	AABBPipelineResource.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	AABBPipelineResource.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_COPY_DEST;
	AABBPipelineResource.Initialize();

	// Describe the geometry, for constructing the bottom level acceleration structures.
	D3D12_RAYTRACING_GEOMETRY_DESC AABBsDescription00{};
	AABBsDescription00.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
	AABBsDescription00.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
	AABBsDescription00.AABBs.AABBCount = AABBCount;
	AABBsDescription00.AABBs.AABBs.StartAddress = AABBPipelineResource.GetInterface()->GetGPUVirtualAddress();
	AABBsDescription00.AABBs.AABBs.StrideInBytes = sizeof(D3D12_RAYTRACING_AABB);

	// Pack the geometry descriptions into an array.
	D3D12_RAYTRACING_GEOMETRY_DESC AABBsDescriptionArray[1]
	{
		AABBsDescription00
	};

	// Describe the desired bottom level acceleration structure.
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS BLASInputs00{};
	BLASInputs00.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	BLASInputs00.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	BLASInputs00.NumDescs = sizeof(AABBsDescriptionArray) / sizeof(D3D12_RAYTRACING_GEOMETRY_DESC);
	BLASInputs00.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	BLASInputs00.InstanceDescs = NULL;																	// Unused for bottom-level.
	BLASInputs00.pGeometryDescs = AABBsDescriptionArray;												// An array of descriptions. (ppGeometryDescs is for an array of pointers to descriptions.)

	// Structure for storing the acceleration structure "prebuild" info.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO BLASPrebuildInfo00{};

	// Collect the acceleration structure prebuild info.
	Device.GetInterface()->GetRaytracingAccelerationStructurePrebuildInfo
	(
		&(BLASInputs00),
		&(BLASPrebuildInfo00)
	);

	// Create and bind a resource for the scratch buffer and the bottom-level acceleration structure itself.
	// BLAS scratch buffer resource.
	const unsigned __int64 BLASScratchResourceByteSize{ BLASPrebuildInfo00.ScratchDataSizeInBytes };

	WD3D12PlacedResource0 BLASScratchResource{};
	BLASScratchResource.InitConfig.unicode_debug_name = L"BLASScratchResource";
	BLASScratchResource.InitConfig.ptr_id3d12heap_v0 = PipelineHeap.GetInterface();
	BLASScratchResource.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	BLASScratchResource.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	BLASScratchResource.InitConfig.heap_offset_in_bytes = 65536 * 1;
	BLASScratchResource.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	BLASScratchResource.InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	BLASScratchResource.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
	BLASScratchResource.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	BLASScratchResource.InitConfig.d3d12_resource_description.Width = BLASScratchResourceByteSize;
	BLASScratchResource.Initialize();

	// BLAS resource.
	const unsigned __int64 BLASResourceByteSize{ BLASPrebuildInfo00.ResultDataMaxSizeInBytes };

	WD3D12PlacedResource0 BLASResource{};
	BLASResource.InitConfig.unicode_debug_name = L"BLASResource";
	BLASResource.InitConfig.ptr_id3d12heap_v0 = PipelineHeap.GetInterface();
	BLASResource.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	BLASResource.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	BLASResource.InitConfig.heap_offset_in_bytes = 65536 * 2;
	BLASResource.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	BLASResource.InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	BLASResource.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
	BLASResource.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	BLASResource.InitConfig.d3d12_resource_description.Width = BLASResourceByteSize;
	BLASResource.Initialize();

	// Build the bottom-level acceleration structure(s).
	// Describe the bottom-level acceleration structure.
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC BLASDescription{};
	BLASDescription.SourceAccelerationStructureData = NULL;					// Only used if doing an incremental update.
	BLASDescription.Inputs = BLASInputs00;
	BLASDescription.ScratchAccelerationStructureData = BLASScratchResource.GetInterface()->GetGPUVirtualAddress();
	BLASDescription.DestAccelerationStructureData = BLASResource.GetInterface()->GetGPUVirtualAddress();





	// Build the top level acceleration structure(s).

	// Instance description(s), for the TLAS build inputs - To be uploaded to GPU memory for usage within the TLAS inputs structure via GPUVirtualAddress.

	// Some values for material indexing.
	UINT LambertianHitGroupIndex{ 0 };
	UINT MetallicHitGroupIndex{ 1 };
	UINT DielectricHitGroupIndex{ 2 };

	// Instance description of the planet sphere.
	UINT PlanetSphereInstanceID{ 1 };				// 1
	UINT PlanetSphereInstanceMask{ 0b0000'0001 };	// 1
	FLOAT PlanetSphereTransform[12]
	{
		+7.0f, 0.0f, 0.0f, 0.0f,
		0.0f, +7.0f, 0.0f, +8.0f,
		0.0f, 0.0f, +7.0f, 0.0f
	};

	D3D12_RAYTRACING_INSTANCE_DESC PlanetSphereInstanceDesc{};
	PlanetSphereInstanceDesc.AccelerationStructure = BLASResource.GetInterface()->GetGPUVirtualAddress();
	PlanetSphereInstanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE;
	PlanetSphereInstanceDesc.InstanceContributionToHitGroupIndex = LambertianHitGroupIndex;
	PlanetSphereInstanceDesc.InstanceID = PlanetSphereInstanceID;
	PlanetSphereInstanceDesc.InstanceMask = PlanetSphereInstanceMask;
	memcpy_s
	(
		PlanetSphereInstanceDesc.Transform,
		sizeof(PlanetSphereTransform),
		PlanetSphereTransform,
		sizeof(PlanetSphereTransform)
	);

	// Instance description of the other sphere.
	UINT MainSphereInstanceID{ 2 };				// 2
	UINT MainSphereInstanceMask{ 0b0000'0010 };	// 2
	FLOAT MainSphereTransform[12]
	{
		+300.0f, 0.0f, 0.0f, 0.0f,
		0.0f, +300.0f, 0.0f, -300.0f,
		0.0f, 0.0f, +300.0f, 0.0f
	};

	D3D12_RAYTRACING_INSTANCE_DESC MainSphereInstanceDesc{};
	MainSphereInstanceDesc.AccelerationStructure = BLASResource.GetInterface()->GetGPUVirtualAddress();
	MainSphereInstanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE;
	MainSphereInstanceDesc.InstanceContributionToHitGroupIndex = LambertianHitGroupIndex;
	MainSphereInstanceDesc.InstanceID = MainSphereInstanceID;
	MainSphereInstanceDesc.InstanceMask = MainSphereInstanceMask;
	memcpy_s
	(
		MainSphereInstanceDesc.Transform,
		sizeof(MainSphereTransform),
		MainSphereTransform,
		sizeof(MainSphereTransform)
	);

	// Pack the instance descriptions into an array.
	const unsigned __int64 InstanceDescriptionCount{ 2 };

	D3D12_RAYTRACING_INSTANCE_DESC InstanceDescriptionArray[InstanceDescriptionCount]{};
	InstanceDescriptionArray[0] = PlanetSphereInstanceDesc;
	InstanceDescriptionArray[1] = MainSphereInstanceDesc;

	// Create an upload resource for the D3D12_RAYTRACING_INSTANCE_DESC array.
	const unsigned __int64 InstanceDescUploadResourceByteSize{ sizeof(InstanceDescriptionArray) };

	WD3D12PlacedResource0 InstanceDescUploadResource{};
	InstanceDescUploadResource.InitConfig.unicode_debug_name = L"InstanceDescUploadResource";
	InstanceDescUploadResource.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	InstanceDescUploadResource.InitConfig.ptr_id3d12heap_v0 = UploadHeap.GetInterface();
	InstanceDescUploadResource.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	InstanceDescUploadResource.InitConfig.heap_offset_in_bytes = 65536;	// 64KB offset, for now.
	InstanceDescUploadResource.InitConfig.d3d12_resource_description.Width = InstanceDescUploadResourceByteSize;
	InstanceDescUploadResource.Initialize();

	// Copy the D3D12_RAYTRACING_INSTANCE_DESC into the upload buffer.
	MemCopyToUploadBuffer
	(
		InstanceDescUploadResource.GetInterface(),
		sizeof(InstanceDescriptionArray),
		InstanceDescriptionArray,
		sizeof(InstanceDescriptionArray)
	);

	// Create a GPU-Only resource, for storing the D3D12_RAYTRACING_INSTANCE_DESC structure.
	const unsigned __int64 InstanceDescPipelineResourceByteSize{ InstanceDescUploadResourceByteSize };

	WD3D12PlacedResource0 InstanceDescPipelineResource{};
	InstanceDescPipelineResource.InitConfig.unicode_debug_name = L"InstanceDescPipelineResource";
	InstanceDescPipelineResource.InitConfig.ptr_id3d12heap_v0 = PipelineHeap.GetInterface();
	InstanceDescPipelineResource.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	InstanceDescPipelineResource.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_COPY_DEST;
	InstanceDescPipelineResource.InitConfig.heap_offset_in_bytes = 65536 * 3;
	InstanceDescPipelineResource.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
	InstanceDescPipelineResource.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	InstanceDescPipelineResource.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	InstanceDescPipelineResource.InitConfig.d3d12_resource_description.Width = InstanceDescPipelineResourceByteSize;
	InstanceDescPipelineResource.Initialize();

	// Inputs for collecting the TLAS PrebuildInfo.
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS TLASInputs00{};
	TLASInputs00.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	TLASInputs00.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	TLASInputs00.InstanceDescs = InstanceDescPipelineResource.GetInterface()->GetGPUVirtualAddress();
	TLASInputs00.NumDescs = InstanceDescriptionCount;
	TLASInputs00.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

	// For storing the collected PrebuildInfo.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO TLASPrebuildInfo{};

	// Collect the TLAS prebuild info, for allocating resources and stuff.
	Device.GetInterface()->GetRaytracingAccelerationStructurePrebuildInfo
	(
		&(TLASInputs00),
		&(TLASPrebuildInfo)
	);

	// Create a TLAS-Scratch and TLAS resource, now that we know their required sizes.
	// TLAS Scratch resource.
	const unsigned __int64 TLASScratchByteSize{ TLASPrebuildInfo.ScratchDataSizeInBytes };

	WD3D12PlacedResource0 TLASScratchResource{};
	TLASScratchResource.InitConfig.unicode_debug_name = L"TLASScratchResource";
	TLASScratchResource.InitConfig.ptr_id3d12heap_v0 = PipelineHeap.GetInterface();
	TLASScratchResource.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	TLASScratchResource.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	TLASScratchResource.InitConfig.heap_offset_in_bytes = 65536 * 4;
	TLASScratchResource.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	TLASScratchResource.InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	TLASScratchResource.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
	TLASScratchResource.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	TLASScratchResource.InitConfig.d3d12_resource_description.Width = TLASScratchByteSize;
	TLASScratchResource.Initialize();

	// TLAS Resource.
	const unsigned __int64 TLASByteSize{ TLASPrebuildInfo.ResultDataMaxSizeInBytes };

	WD3D12PlacedResource0 TLASResource{};
	TLASResource.InitConfig.unicode_debug_name = L"TLASResource";
	TLASResource.InitConfig.ptr_id3d12heap_v0 = PipelineHeap.GetInterface();
	TLASResource.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	TLASResource.InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	TLASResource.InitConfig.heap_offset_in_bytes = 65536 * 5;
	TLASResource.InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	TLASResource.InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	TLASResource.InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
	TLASResource.InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	TLASResource.InitConfig.d3d12_resource_description.Width = TLASByteSize;
	TLASResource.Initialize();

	// Describe the TLAS.
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC BuildTLASDescription{};
	BuildTLASDescription.SourceAccelerationStructureData = NULL;
	BuildTLASDescription.ScratchAccelerationStructureData = TLASScratchResource.GetInterface()->GetGPUVirtualAddress();
	BuildTLASDescription.DestAccelerationStructureData = TLASResource.GetInterface()->GetGPUVirtualAddress();
	BuildTLASDescription.Inputs = TLASInputs00;

	// Describe a shader resource view of the final ray tracing acceleration structure(s), to be used for binding it to the pipeline.
	D3D12_SHADER_RESOURCE_VIEW_DESC RASShaderResourceViewDescription{};
	RASShaderResourceViewDescription.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	RASShaderResourceViewDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	RASShaderResourceViewDescription.RaytracingAccelerationStructure.Location = TLASResource.GetInterface()->GetGPUVirtualAddress();

	// Use the DescriptorHandleIncrementSize, an offset value, and the heap start, for indexing into the descriptor heap, where we will store the created shader resource view.
	D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandleToRASSRV{};
	D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandleToRASSRV{};

	GetDescriptorHandles
	(
		DescriptorHeap.GetInterface(),
		DescriptorHeapOffset,
		DescriptorHandleIncrementSize,
		&CPUDescriptorHandleToRASSRV,
		&GPUDescriptorHandleToRASSRV
	);

	DescriptorHeapOffset++;

	Device.GetInterface()->CreateShaderResourceView
	(
		nullptr,
		&RASShaderResourceViewDescription,
		CPUDescriptorHandleToRASSRV
	);




	// Create the ray tracing pipeline state.

	// Global root signature + state subobject.

	// Shader resource view of the RAS.
	D3D12_ROOT_DESCRIPTOR1 SRVRootDescriptor{};
	SRVRootDescriptor.RegisterSpace = 0;
	SRVRootDescriptor.ShaderRegister = 0;

	// Descriptor table containing a UAV of the intersection map.
	D3D12_DESCRIPTOR_RANGE1 UAVDescriptorRange[1]{};
	UAVDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	UAVDescriptorRange[0].NumDescriptors = 2;
	UAVDescriptorRange[0].BaseShaderRegister = 0;
	UAVDescriptorRange[0].RegisterSpace = 0;
	UAVDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE1 UAVDescriptorTable{};
	UAVDescriptorTable.NumDescriptorRanges = 1;
	UAVDescriptorTable.pDescriptorRanges = UAVDescriptorRange;

	// Inline root constants.
	D3D12_ROOT_CONSTANTS InlineConstants{};
	InlineConstants.Num32BitValues = InlineConstantsCount;
	InlineConstants.RegisterSpace = 0;
	InlineConstants.ShaderRegister = 0;

	// Define the root parameters within an array.
	D3D12_ROOT_PARAMETER1 GlobalRootParameters[3]{};
	GlobalRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	GlobalRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	GlobalRootParameters[0].Descriptor = SRVRootDescriptor;

	GlobalRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	GlobalRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	GlobalRootParameters[1].DescriptorTable = UAVDescriptorTable;

	GlobalRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	GlobalRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	GlobalRootParameters[2].Constants = InlineConstants;

	// Describe the global root signature.
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC GlobalRootSignatureDescription{};
	GlobalRootSignatureDescription.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
	GlobalRootSignatureDescription.Desc_1_1.NumParameters = 3;
	GlobalRootSignatureDescription.Desc_1_1.pParameters = GlobalRootParameters;

	ID3DBlob* GlobalRootSignatureBlob{};
	ID3DBlob* ErrorBlob{};

	WD3D12::FailCheck
	(
		D3D12SerializeVersionedRootSignature
		(
			&GlobalRootSignatureDescription,
			&GlobalRootSignatureBlob,
			&ErrorBlob
		),
		L"D3D12SerializeVersionedRootSignature() failed. (Global)",
		L"D3D12SerializeVersionedRootSignature() failed. (Global)"
	);

	WD3D12RootSignature0 IGlobalRootSignature{};
	IGlobalRootSignature.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	IGlobalRootSignature.InitConfig.ptr_id3dblob_v0 = GlobalRootSignatureBlob;
	IGlobalRootSignature.Initialize();

	D3D12_GLOBAL_ROOT_SIGNATURE GlobalRootSignature{};
	GlobalRootSignature.pGlobalRootSignature = IGlobalRootSignature.GetInterface();

	D3D12_STATE_SUBOBJECT GlobalRootSignatureSubobject{};
	GlobalRootSignatureSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
	GlobalRootSignatureSubobject.pDesc = &GlobalRootSignature;

	// Ray generation shader export + DXIL library + state subobject.
	const wchar_t* Name_RayGenerationShader{ L"RayGeneration" };

	D3D12_SHADER_BYTECODE RayGenerationShaderByteCode{};
	RayGenerationShaderByteCode.pShaderBytecode = (void*)RayGeneration;
	RayGenerationShaderByteCode.BytecodeLength = sizeof(RayGeneration);

	D3D12_EXPORT_DESC RayGenerationShaderExportDescription[1]{};
	RayGenerationShaderExportDescription[0].Name = Name_RayGenerationShader;
	RayGenerationShaderExportDescription[0].ExportToRename = nullptr;
	RayGenerationShaderExportDescription[0].Flags = D3D12_EXPORT_FLAG_NONE;

	D3D12_DXIL_LIBRARY_DESC RayGenerationShaderLibDescription[1]{};
	RayGenerationShaderLibDescription[0].DXILLibrary = RayGenerationShaderByteCode;
	RayGenerationShaderLibDescription[0].NumExports = 1;
	RayGenerationShaderLibDescription[0].pExports = RayGenerationShaderExportDescription;

	D3D12_STATE_SUBOBJECT RayGenerationShaderSubobject{};
	RayGenerationShaderSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	RayGenerationShaderSubobject.pDesc = RayGenerationShaderLibDescription;

	// Lambertian intersection shader export + DXIL library + state subobject.
	const wchar_t* Name_LambertianIntersectionShader{ L"LambertianIntersection" };

	D3D12_SHADER_BYTECODE LambertianIntersectionShaderByteCode{};
	LambertianIntersectionShaderByteCode.pShaderBytecode = (void*)LambertianIntersection;
	LambertianIntersectionShaderByteCode.BytecodeLength = sizeof(LambertianIntersection);

	D3D12_EXPORT_DESC LambertianIntersectionShaderExportDescription[1]{};
	LambertianIntersectionShaderExportDescription[0].Name = Name_LambertianIntersectionShader;
	LambertianIntersectionShaderExportDescription[0].ExportToRename = nullptr;
	LambertianIntersectionShaderExportDescription[0].Flags = D3D12_EXPORT_FLAG_NONE;

	D3D12_DXIL_LIBRARY_DESC LambertianIntersectionShaderLibDescription[1]{};
	LambertianIntersectionShaderLibDescription[0].DXILLibrary = LambertianIntersectionShaderByteCode;
	LambertianIntersectionShaderLibDescription[0].NumExports = 1;
	LambertianIntersectionShaderLibDescription[0].pExports = LambertianIntersectionShaderExportDescription;

	D3D12_STATE_SUBOBJECT LambertianIntersectionShaderSubobject{};
	LambertianIntersectionShaderSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	LambertianIntersectionShaderSubobject.pDesc = LambertianIntersectionShaderLibDescription;

	// Lambertian closest hit shader export + DXIL library + state subobject.
	const wchar_t* Name_LambertianClosestHitShader{ L"LambertianClosestHit" };

	D3D12_SHADER_BYTECODE LambertianClosestHitShaderByteCode{};
	LambertianClosestHitShaderByteCode.pShaderBytecode = (void*)LambertianClosestHit;
	LambertianClosestHitShaderByteCode.BytecodeLength = sizeof(LambertianClosestHit);

	D3D12_EXPORT_DESC LambertianClosestHitShaderExportDescription[1]{};
	LambertianClosestHitShaderExportDescription[0].Name = Name_LambertianClosestHitShader;
	LambertianClosestHitShaderExportDescription[0].ExportToRename = nullptr;
	LambertianClosestHitShaderExportDescription[0].Flags = D3D12_EXPORT_FLAG_NONE;

	D3D12_DXIL_LIBRARY_DESC LambertianClosestHitShaderLibDescription[1]{};
	LambertianClosestHitShaderLibDescription[0].DXILLibrary = LambertianClosestHitShaderByteCode;
	LambertianClosestHitShaderLibDescription[0].NumExports = 1;
	LambertianClosestHitShaderLibDescription[0].pExports = LambertianClosestHitShaderExportDescription;

	D3D12_STATE_SUBOBJECT LambertianClosestHitShaderSubobject{};
	LambertianClosestHitShaderSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	LambertianClosestHitShaderSubobject.pDesc = LambertianClosestHitShaderLibDescription;

	// Lambertian miss shader export + DXIL library + state subobject.
	const wchar_t* Name_LambertianMissShader{ L"LambertianMiss" };

	D3D12_SHADER_BYTECODE LambertianMissShaderByteCode{};
	LambertianMissShaderByteCode.pShaderBytecode = (void*)LambertianMiss;
	LambertianMissShaderByteCode.BytecodeLength = sizeof(LambertianMiss);

	D3D12_EXPORT_DESC LambertianMissShaderExportDescription[1]{};
	LambertianMissShaderExportDescription[0].Name = Name_LambertianMissShader;
	LambertianMissShaderExportDescription[0].ExportToRename = nullptr;
	LambertianMissShaderExportDescription[0].Flags = D3D12_EXPORT_FLAG_NONE;

	D3D12_DXIL_LIBRARY_DESC LambertianMissShaderLibDescription[1]{};
	LambertianMissShaderLibDescription[0].DXILLibrary = LambertianMissShaderByteCode;
	LambertianMissShaderLibDescription[0].NumExports = 1;
	LambertianMissShaderLibDescription[0].pExports = LambertianMissShaderExportDescription;

	D3D12_STATE_SUBOBJECT LambertianMissShaderSubobject{};
	LambertianMissShaderSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	LambertianMissShaderSubobject.pDesc = LambertianMissShaderLibDescription;

	// Lambertian hit group + state subobject.
	const wchar_t* Name_LambertianHitGroup{ L"LambertianHitGroup" };

	D3D12_HIT_GROUP_DESC LambertianHitGroupDescription[1]{};
	LambertianHitGroupDescription[0].Type = D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;
	LambertianHitGroupDescription[0].IntersectionShaderImport = Name_LambertianIntersectionShader;
	LambertianHitGroupDescription[0].ClosestHitShaderImport = Name_LambertianClosestHitShader;
	LambertianHitGroupDescription[0].HitGroupExport = Name_LambertianHitGroup;

	D3D12_STATE_SUBOBJECT LambertianHitGroupSubobject{};
	LambertianHitGroupSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
	LambertianHitGroupSubobject.pDesc = LambertianHitGroupDescription;

	// Raytracing shader config + state subobject.
	D3D12_RAYTRACING_SHADER_CONFIG RaytracingShaderConfig{};
	RaytracingShaderConfig.MaxPayloadSizeInBytes = 32U;
	RaytracingShaderConfig.MaxAttributeSizeInBytes = 24U;

	D3D12_STATE_SUBOBJECT RaytracingShaderConfigSubobject{};
	RaytracingShaderConfigSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
	RaytracingShaderConfigSubobject.pDesc = &RaytracingShaderConfig;

	// Raytracing pipeline config + state subobject.
	D3D12_RAYTRACING_PIPELINE_CONFIG RaytracingPipelineConfig{};
	RaytracingPipelineConfig.MaxTraceRecursionDepth = InlineConstantBuffer.MaxRecursionDepth;

	D3D12_STATE_SUBOBJECT RaytracingPipelineConfigSubobject{};
	RaytracingPipelineConfigSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
	RaytracingPipelineConfigSubobject.pDesc = &RaytracingPipelineConfig;

	// Describe the pipeline state object.
	D3D12_STATE_SUBOBJECT StateSubobjects[8]
	{
		RayGenerationShaderSubobject,
		LambertianIntersectionShaderSubobject,
		LambertianClosestHitShaderSubobject,
		LambertianMissShaderSubobject,
		LambertianHitGroupSubobject,
		RaytracingShaderConfigSubobject,
		RaytracingPipelineConfigSubobject,
		GlobalRootSignatureSubobject
	};

	WD3D12StateObject0 StateObject_RaytracingPipeline{};
	StateObject_RaytracingPipeline.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	StateObject_RaytracingPipeline.InitConfig.d3d12_state_object_description.NumSubobjects = sizeof(StateSubobjects) / sizeof(D3D12_STATE_SUBOBJECT);
	StateObject_RaytracingPipeline.InitConfig.d3d12_state_object_description.pSubobjects = StateSubobjects;
	StateObject_RaytracingPipeline.Initialize();

	// Collect the shader Identifiers, for constructing the shader table.
	ID3D12StateObjectProperties* IStateObjectProperties{};

	WD3D12::FailCheck
	(
		StateObject_RaytracingPipeline.GetInterface()->QueryInterface
		(
			&IStateObjectProperties
		),
		L"StateObject_RaytracingPipeline.GetInterface()->QueryInterface() failed.",
		L"StateObject_RaytracingPipeline.GetInterface()->QueryInterface() failed."
	);

	// Shader identifiers.
	void* RayGenerationShaderIdentifier{};
	RayGenerationShaderIdentifier = IStateObjectProperties->GetShaderIdentifier
	(
		Name_RayGenerationShader
	);

	void* HitGroupShaderIdentifier{};
	HitGroupShaderIdentifier = IStateObjectProperties->GetShaderIdentifier
	(
		Name_LambertianHitGroup
	);

	void* MissShaderIdentifier{};
	MissShaderIdentifier = IStateObjectProperties->GetShaderIdentifier
	(
		Name_LambertianMissShader
	);

	// Ray generation shader table.
	WD3D12CommittedResource0 RayGenerationShaderTableBuffer{};
	RayGenerationShaderTableBuffer.InitConfig.unicode_debug_name = L"RayGenerationShaderTableBuffer";
	RayGenerationShaderTableBuffer.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	RayGenerationShaderTableBuffer.InitConfig.d3d12_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	RayGenerationShaderTableBuffer.InitConfig.d3d12_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	RayGenerationShaderTableBuffer.InitConfig.d3d12_resource_description.Width = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	RayGenerationShaderTableBuffer.InitConfig.d3d12_resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	RayGenerationShaderTableBuffer.Initialize();

	// Copy the shader table into the upload buffer.
	MemCopyToUploadBuffer
	(
		RayGenerationShaderTableBuffer.GetInterface(),
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,
		RayGenerationShaderIdentifier,
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES
	);

	// Hit group shader table.
	WD3D12CommittedResource0 HitGroupShaderTableBuffer{};
	HitGroupShaderTableBuffer.InitConfig.unicode_debug_name = L"HitGroupShaderTableBuffer";
	HitGroupShaderTableBuffer.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	HitGroupShaderTableBuffer.InitConfig.d3d12_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	HitGroupShaderTableBuffer.InitConfig.d3d12_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	HitGroupShaderTableBuffer.InitConfig.d3d12_resource_description.Width = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	HitGroupShaderTableBuffer.InitConfig.d3d12_resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	HitGroupShaderTableBuffer.Initialize();

	// Copy the shader table into the upload buffer.
	MemCopyToUploadBuffer
	(
		HitGroupShaderTableBuffer.GetInterface(),
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,
		HitGroupShaderIdentifier,
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES
	);

	// Miss shader table.
	WD3D12CommittedResource0 MissShaderTableBuffer{};
	MissShaderTableBuffer.InitConfig.unicode_debug_name = L"MissShaderTableBuffer";
	MissShaderTableBuffer.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	MissShaderTableBuffer.InitConfig.d3d12_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	MissShaderTableBuffer.InitConfig.d3d12_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	MissShaderTableBuffer.InitConfig.d3d12_resource_description.Width = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	MissShaderTableBuffer.InitConfig.d3d12_resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	MissShaderTableBuffer.Initialize();

	// Copy the shader table into the upload buffer.
	MemCopyToUploadBuffer
	(
		MissShaderTableBuffer.GetInterface(),
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,
		MissShaderIdentifier,
		D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES
	);

	// Fill out a description of the DispatchRays call.
	D3D12_DISPATCH_RAYS_DESC DispatchRaysDescription{};
	DispatchRaysDescription.Width = PixelWidth;
	DispatchRaysDescription.Height = PixelHeight;
	DispatchRaysDescription.Depth = 1;
	DispatchRaysDescription.RayGenerationShaderRecord.StartAddress = RayGenerationShaderTableBuffer.GetInterface()->GetGPUVirtualAddress();
	DispatchRaysDescription.RayGenerationShaderRecord.SizeInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	DispatchRaysDescription.HitGroupTable.StartAddress = HitGroupShaderTableBuffer.GetInterface()->GetGPUVirtualAddress();
	DispatchRaysDescription.HitGroupTable.SizeInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	DispatchRaysDescription.HitGroupTable.StrideInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	DispatchRaysDescription.MissShaderTable.StartAddress = MissShaderTableBuffer.GetInterface()->GetGPUVirtualAddress();
	DispatchRaysDescription.MissShaderTable.SizeInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	DispatchRaysDescription.MissShaderTable.StrideInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	


	
	// Record a list of commands to prepare for rendering.
	WD3D12GraphicsCommandList4 InitializationCommandList{};
	InitializationCommandList.InitConfig.unicode_debug_name = L"InitializationCommandList";
	InitializationCommandList.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	InitializationCommandList.InitConfig.ptr_id3d12commandallocator_v0 = DirectCommandAllocator.GetInterface();
	InitializationCommandList.InitConfig.d3d12_command_list_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	InitializationCommandList.Initialize();

	// Copy from the intersection map upload buffer, to the dedicated GPU intersection map.
	InitializationCommandList.GetInterface()->CopyTextureRegion
	(
		&TextureCopyLocationIntersectionMap2DTexture,
		0,
		0,
		0,
		&TextureCopyLocationIntersectionMapUploadResource,
		nullptr
	);

	// Prepare the resource state for shader access.
	InitializationCommandList.GetInterface()->ResourceBarrier
	(
		1,
		&(CreateResourceTransitionBarrier
		(
			IntersectionMap2DTexture.GetInterface(),
			0,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		)
			)
	);

	// Copy the random numbers from shared memory to dedicated GPU memory and update the resource state for shader access.
	InitializationCommandList.GetInterface()->CopyResource(
		RandomNumberBuffer.GetInterface(),
		RandomNumberUploadBuffer.GetInterface()
	);

	InitializationCommandList.GetInterface()->ResourceBarrier(
		1,
		&(CreateResourceTransitionBarrier(
			RandomNumberBuffer.GetInterface(),
			0,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		)
			)
	);

	// Copy the AABB data from shared to dedicated GPU memory and update its resource state for building the BLAS.
	InitializationCommandList.GetInterface()->CopyResource
	(
		AABBPipelineResource.GetInterface(),
		AABBUploadResource.GetInterface()
	);

	InitializationCommandList.GetInterface()->ResourceBarrier
	(
		1,
		&(CreateResourceTransitionBarrier
		(
			AABBPipelineResource.GetInterface(),
			0,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
		)
			)
	);

	// Build the bottom level acceleration structure(s).
	InitializationCommandList.GetInterface()->BuildRaytracingAccelerationStructure
	(
		&BLASDescription,
		0,
		nullptr
	);

	// Build the top level acceleration structure.
	InitializationCommandList.GetInterface()->CopyResource
	(
		InstanceDescPipelineResource.GetInterface(),
		InstanceDescUploadResource.GetInterface()
	);

	InitializationCommandList.GetInterface()->ResourceBarrier
	(
		1,
		&(CreateResourceTransitionBarrier
		(
			InstanceDescPipelineResource.GetInterface(),
			0,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
		)
			)
	);

	InitializationCommandList.GetInterface()->BuildRaytracingAccelerationStructure
	(
		&BuildTLASDescription,
		0,
		nullptr
	);

	// Set the global root signature.
	InitializationCommandList.GetInterface()->SetComputeRootSignature
	(
		IGlobalRootSignature.GetInterface()
	);

	// Set the descriptor heap.
	ID3D12DescriptorHeap* pDescriptorHeap{ DescriptorHeap.GetInterface() };
	InitializationCommandList.GetInterface()->SetDescriptorHeaps
	(
		1,
		&pDescriptorHeap
	);

	// Set a root descriptor.
	InitializationCommandList.GetInterface()->SetComputeRootShaderResourceView
	(
		0,
		TLASResource.GetInterface()->GetGPUVirtualAddress()
	);

	// Set the descriptor table.
	InitializationCommandList.GetInterface()->SetComputeRootDescriptorTable
	(
		1,
		DescriptorHeap.GetInterface()->GetGPUDescriptorHandleForHeapStart()
	);

	// Set the global inline root constants.
	InitializationCommandList.GetInterface()->SetComputeRoot32BitConstants
	(
		2,
		InlineConstantsCount,
		&InlineConstantBuffer,
		0
	);

	// Set the pipeline state.
	InitializationCommandList.GetInterface()->SetPipelineState1
	(
		StateObject_RaytracingPipeline.GetInterface()
	);

	// Time to dispatch some rays.
	InitializationCommandList.GetInterface()->DispatchRays
	(
		&(DispatchRaysDescription)
	);

	// Change some resource states.
	D3D12_RESOURCE_BARRIER UAVBarrier{};
	UAVBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	UAVBarrier.UAV.pResource = IntersectionMap2DTexture.GetInterface();

	InitializationCommandList.GetInterface()->ResourceBarrier
	(
		1,
		&UAVBarrier
	);

	InitializationCommandList.GetInterface()->ResourceBarrier
	(
		1,
		&CreateResourceTransitionBarrier
		(
			IntersectionMap2DTexture.GetInterface(),
			0,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COPY_SOURCE
		)
	);

	// Close and submit the list of commands, and wait for completion.
	InitializationCommandList.GetInterface()->Close();

	ID3D12CommandList* CommandList[1]{};
	CommandList[0] = InitializationCommandList.GetInterface();

	DirectCommandQueue.GetInterface()->ExecuteCommandLists
	(
		sizeof(CommandList) / sizeof(ID3D12CommandList*),
		CommandList
	);

	FlushCommandQueue
	(
		DirectCommandQueue.GetInterface(),
		Fence.GetInterface(),
		&FenceValue,
		FenceIncrement
	);





	// Win32 window and DXGI swap chain for presenting the rendered frame.
	Win32Window Window{};
	Window.InitConfig.Width = PixelWidth;
	Window.InitConfig.Height = PixelHeight;
	Window.Initialize();

	using namespace WDXGI;

	WDXGIFactory7 DXGIFactory{};
	DXGIFactory.Initialize();

	WDXGISwapChain4 SwapChain{};
	SwapChain.InitConfig.ptr_idxgifactory_v7 = DXGIFactory.GetInterface();
	SwapChain.InitConfig.ptr_id3d12commandqueue_v0 = DirectCommandQueue.GetInterface();
	SwapChain.InitConfig.dxgi_swap_chain_description_v1.Width = PixelWidth;
	SwapChain.InitConfig.dxgi_swap_chain_description_v1.Height = PixelHeight;
	SwapChain.InitConfig.win32_window_handle = Window.GetWindowHandle();
	SwapChain.Initialize();

	ID3D12Resource* BackBuffer00{};
	ID3D12Resource* BackBuffer01{};

	SwapChain.GetInterface()->GetBuffer
	(
		0,
		__uuidof(ID3D12Resource),
		(void**) & BackBuffer00
	);

	FlushCommandQueue
	(
		DirectCommandQueue.GetInterface(),
		Fence.GetInterface(),
		&FenceValue,
		FenceIncrement
	);

	SwapChain.GetInterface()->Present
	(
		1,
		NULL
	);

	SwapChain.GetInterface()->GetBuffer
	(
		1,
		__uuidof(ID3D12Resource),
		(void**) & BackBuffer01
	);

	FlushCommandQueue
	(
		DirectCommandQueue.GetInterface(),
		Fence.GetInterface(),
		&FenceValue,
		FenceIncrement
	);





	// Record the lists of presentation commands.
	ID3D12CommandList* FrameCommandLists[2]{};

	WD3D12GraphicsCommandList4 FrameCommandList00{};
	FrameCommandList00.InitConfig.unicode_debug_name = L"FrameCommandList00";
	FrameCommandList00.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	FrameCommandList00.InitConfig.ptr_id3d12commandallocator_v0 = DirectCommandAllocator.GetInterface();
	FrameCommandList00.InitConfig.ptr_id3d12pipelinestate_v0 = nullptr;
	FrameCommandList00.InitConfig.d3d12_command_list_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	FrameCommandList00.InitConfig.node_mask = 0;
	FrameCommandList00.Initialize();

	FrameCommandList00.GetInterface()->ResourceBarrier
	(
		1,
		&CreateResourceTransitionBarrier
		(
			BackBuffer00,
			0,
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST
		)
	);

	FrameCommandList00.GetInterface()->CopyResource
	(
		BackBuffer00,
		IntersectionMap2DTexture.GetInterface()
	);

	FrameCommandList00.GetInterface()->ResourceBarrier
	(
		1,
		&CreateResourceTransitionBarrier
		(
			BackBuffer00,
			0,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PRESENT
		)
	);

	FrameCommandList00.GetInterface()->Close();

	FrameCommandLists[0] = FrameCommandList00.GetInterface();

	WD3D12GraphicsCommandList4 FrameCommandList01{};
	FrameCommandList01.InitConfig.unicode_debug_name = L"FrameCommandList01";
	FrameCommandList01.InitConfig.ptr_id3d12device_v5 = Device.GetInterface();
	FrameCommandList01.InitConfig.ptr_id3d12commandallocator_v0 = DirectCommandAllocator.GetInterface();
	FrameCommandList01.InitConfig.ptr_id3d12pipelinestate_v0 = nullptr;
	FrameCommandList01.InitConfig.d3d12_command_list_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	FrameCommandList01.InitConfig.node_mask = 0;
	FrameCommandList01.Initialize();

	FrameCommandList01.GetInterface()->ResourceBarrier
	(
		1,
		&CreateResourceTransitionBarrier
		(
			BackBuffer01,
			0,
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST
		)
	);

	FrameCommandList01.GetInterface()->CopyResource
	(
		BackBuffer01,
		IntersectionMap2DTexture.GetInterface()
	);

	FrameCommandList01.GetInterface()->ResourceBarrier
	(
		1,
		&CreateResourceTransitionBarrier
		(
			BackBuffer01,
			0,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PRESENT
		)
	);

	FrameCommandList01.GetInterface()->Close();

	FrameCommandLists[1] = FrameCommandList01.GetInterface();






	// Main event loop of the application.
	MSG MessageStruct{};
	UINT FrameIndex{};	

	ShowCursor
	(
		FALSE
	);

	while (1 == 1)
	{
		PeekMessageW
		(
			&(MessageStruct),
			NULL,
			0,
			0,
			PM_REMOVE
		);

		if (MessageStruct.message == WM_LBUTTONDOWN)
		{
			break;
		}
		else if (MessageStruct.message == WM_PAINT)
		{
			FrameIndex = SwapChain.GetInterface()->GetCurrentBackBufferIndex();

			DirectCommandQueue.GetInterface()->ExecuteCommandLists
			(
				1,
				&(FrameCommandLists[FrameIndex])
			);

			FlushCommandQueue
			(
				DirectCommandQueue.GetInterface(),
				Fence.GetInterface(),
				&FenceValue,
				FenceIncrement
			);

			SwapChain.GetInterface()->Present
			(
				1,
				NULL
			);
		}
		else if (IsDialogMessageW(Window.GetWindowHandle(), &(MessageStruct)))
		{
			// Default Windows dialog message handler.
		}
		else
		{
			TranslateMessage(&(MessageStruct));
			DispatchMessageW(&(MessageStruct));
		}
	}

	ShowCursor
	(
		TRUE
	);

	// Report the debug data.
	using namespace WDXGI;

	WDXGIDebug1 DXGIDebug{};
	DXGIDebug.InitConfig.EnableDebugLayer = DebugSwitch;
	DXGIDebug.Initialize();

	return 0;
}
