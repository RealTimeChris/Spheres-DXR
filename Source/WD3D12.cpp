// WD3D12.cpp - Wrappers for some of the core Direct3D 12 interfaces.
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#include "WD3D12.hpp"

// Wrappers for some of the core Direct3D 12 interfaces.
namespace WD3D12
{
	// WD3D12Debug0 class.
	WD3D12Debug0::WD3D12Debug0
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12debug_v0 = nullptr;
		this->Config.unicode_name = L"WD3D12Debug0";
		this->Config.unicode_error_message = L"WD3D12Debug0.Initialize() failed.";

		this->InitConfig.enable_debug_layer = true;
	}

	void WD3D12Debug0::Initialize
	()
	{
		FailCheck
		(
			D3D12GetDebugInterface
			(
				__uuidof(ID3D12Debug),
				(void**) & (this->Config.ptr_id3d12debug_v0)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		if (this->InitConfig.enable_debug_layer == true)
		{
			this->Config.ptr_id3d12debug_v0->EnableDebugLayer();
		}
	}

	ID3D12Debug* WD3D12Debug0::GetInterface
	()
	{
		return this->Config.ptr_id3d12debug_v0;
	}

	WD3D12Debug0::~WD3D12Debug0
	()
	{
		if (this->Config.ptr_id3d12debug_v0 != nullptr)
		{
			this->Config.ptr_id3d12debug_v0->Release();
			this->Config.ptr_id3d12debug_v0 = nullptr;
		}		
	}





	// WD3D12Device5 class.
	WD3D12Device5::WD3D12Device5
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12device_v5 = nullptr;
		this->Config.unicode_name = L"WD3D12Device5";
		this->Config.unicode_error_message = L"WD3D12Device5.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"Device5_01";
		this->InitConfig.ptr_adapter = nullptr;
		this->InitConfig.d3d_feature_level = D3D_FEATURE_LEVEL_12_1;
	}

	void WD3D12Device5::Initialize
	()
	{
		FailCheck
		(
			D3D12CreateDevice
			(
				this->InitConfig.ptr_adapter,
				this->InitConfig.d3d_feature_level,
				__uuidof(ID3D12Device5),
				(void**) & (this->Config.ptr_id3d12device_v5)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		FailCheck
		(
			this->Config.ptr_id3d12device_v5->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12Device5* WD3D12Device5::GetInterface
	()
	{
		return this->Config.ptr_id3d12device_v5;
	}

	WD3D12Device5::~WD3D12Device5
	()
	{
		if (this->Config.ptr_id3d12device_v5 != nullptr)
		{
			this->Config.ptr_id3d12device_v5->Release();
			this->Config.ptr_id3d12device_v5 = nullptr;
		}		
	}





	// WD3D12CommandQueue0 class.
	WD3D12CommandQueue0::WD3D12CommandQueue0
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12commandqueue_v0 = nullptr;
		this->Config.unicode_name = L"WD3D12CommandQueue0";
		this->Config.unicode_error_message = L"WD3D12CommandQueue0.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"CommandQueue0_01";

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		this->InitConfig.d3d12_command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		this->InitConfig.d3d12_command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		this->InitConfig.d3d12_command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		this->InitConfig.d3d12_command_queue_desc.NodeMask = 0;		
	}

	void WD3D12CommandQueue0::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreateCommandQueue
			(
				&(this->InitConfig.d3d12_command_queue_desc),
				__uuidof(ID3D12CommandQueue),
				(void**) & (this->Config.ptr_id3d12commandqueue_v0)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12commandqueue_v0->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12CommandQueue* WD3D12CommandQueue0::GetInterface
	()
	{
		return this->Config.ptr_id3d12commandqueue_v0;
	}

	WD3D12CommandQueue0::~WD3D12CommandQueue0
	()
	{
		if (this->Config.ptr_id3d12commandqueue_v0 != nullptr)
		{
			this->Config.ptr_id3d12commandqueue_v0->Release();
			this->Config.ptr_id3d12commandqueue_v0 = nullptr;
		}		
	}





	// WD3D12CommandAllocator0 class.
	WD3D12CommandAllocator0::WD3D12CommandAllocator0
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12commandallocator_v0 = nullptr;
		this->Config.unicode_name = L"WD3D12CommandAllocator0";
		this->Config.unicode_error_message = L"WD3D12CommandAllocator0.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"CommandAllocator0_01";
		this->InitConfig.ptr_id3d12device_v5 = nullptr;
		this->InitConfig.d3d12_command_list_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	}

	void WD3D12CommandAllocator0::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreateCommandAllocator
			(
				this->InitConfig.d3d12_command_list_type,
				__uuidof(ID3D12CommandAllocator),
				(void**) & (this->Config.ptr_id3d12commandallocator_v0)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12commandallocator_v0->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12CommandAllocator* WD3D12CommandAllocator0::GetInterface
	()
	{
		return this->Config.ptr_id3d12commandallocator_v0;
	}

	WD3D12CommandAllocator0::~WD3D12CommandAllocator0
	()
	{
		if (this->Config.ptr_id3d12commandallocator_v0 != nullptr)
		{
			this->Config.ptr_id3d12commandallocator_v0->Release();
			this->Config.ptr_id3d12commandallocator_v0 = nullptr;
		}		
	}





	// WD3D12GraphicsCommandList4 class.
	WD3D12GraphicsCommandList4::WD3D12GraphicsCommandList4
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12graphicscommandlist_v4 = nullptr;
		this->Config.unicode_name = L"WD3D12GraphicsCommandList4";
		this->Config.unicode_error_message = L"WD3D12GraphicsCommandList4.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"GraphicsCommandList4_01";

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		this->InitConfig.node_mask = 0;
		this->InitConfig.d3d12_command_list_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		this->InitConfig.ptr_id3d12commandallocator_v0 = nullptr;
		this->InitConfig.ptr_id3d12pipelinestate_v0 = nullptr;
	}

	void WD3D12GraphicsCommandList4::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreateCommandList
			(
				this->InitConfig.node_mask,
				this->InitConfig.d3d12_command_list_type,
				this->InitConfig.ptr_id3d12commandallocator_v0,
				this->InitConfig.ptr_id3d12pipelinestate_v0,
				__uuidof(ID3D12GraphicsCommandList4),
				(void**) & (this->Config.ptr_id3d12graphicscommandlist_v4)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3d12commandallocator_v0 = nullptr;
		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12graphicscommandlist_v4->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12GraphicsCommandList4* WD3D12GraphicsCommandList4::GetInterface
	()
	{
		return this->Config.ptr_id3d12graphicscommandlist_v4;
	}

	void WD3D12GraphicsCommandList4::ReleaseManually
	()
	{
		if (this->Config.ptr_id3d12graphicscommandlist_v4 != nullptr)
		{
			this->Config.ptr_id3d12graphicscommandlist_v4->Release();
			this->Config.ptr_id3d12graphicscommandlist_v4 = nullptr;
		}
	}

	WD3D12GraphicsCommandList4::~WD3D12GraphicsCommandList4
	()
	{
		if (this->Config.ptr_id3d12graphicscommandlist_v4 != nullptr)
		{
			this->Config.ptr_id3d12graphicscommandlist_v4->Release();
			this->Config.ptr_id3d12graphicscommandlist_v4 = nullptr;
		}
	}





	// WD3D12Fence1 class.
	WD3D12Fence1::WD3D12Fence1
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12fence_v1 = nullptr;
		this->Config.unicode_name = L"WD3D12Fence1";
		this->Config.unicode_error_message = L"WD3D12Fence1.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"Fence1_01";
		this->InitConfig.fence_initial_value = 0;
		this->InitConfig.d3d12_fence_flags = D3D12_FENCE_FLAG_NONE;
	}

	void WD3D12Fence1::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreateFence
			(
				this->InitConfig.fence_initial_value,
				this->InitConfig.d3d12_fence_flags,
				__uuidof(ID3D12Fence1),
				(void**) & (this->Config.ptr_id3d12fence_v1)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12fence_v1->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12Fence1* WD3D12Fence1::GetInterface
	()
	{
		return this->Config.ptr_id3d12fence_v1;
	}

	void WD3D12Fence1::ReleaseManually
	()
	{
		if (this->Config.ptr_id3d12fence_v1 != nullptr)
		{
			this->Config.ptr_id3d12fence_v1->Release();
			this->Config.ptr_id3d12fence_v1 = nullptr;
		}		
	}

	WD3D12Fence1::~WD3D12Fence1
	()
	{
		if (this->Config.ptr_id3d12fence_v1 != nullptr)
		{
			this->Config.ptr_id3d12fence_v1->Release();
			this->Config.ptr_id3d12fence_v1 = nullptr;
		}
	}





	// WD3D12DescriptorHeap0 class.
	WD3D12DescriptorHeap0::WD3D12DescriptorHeap0
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12descriptorheap_v0 = nullptr;
		this->Config.unicode_name = L"WD3D12DescriptorHeap0";
		this->Config.unicode_error_message = L"WD3D12DescriptorHeap0.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"DescriptorHeap0_01";

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		this->InitConfig.id3d12_descriptor_heap_description.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		this->InitConfig.id3d12_descriptor_heap_description.NumDescriptors = 0;
		this->InitConfig.id3d12_descriptor_heap_description.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		this->InitConfig.id3d12_descriptor_heap_description.NodeMask = 0;
	}

	void WD3D12DescriptorHeap0::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreateDescriptorHeap
			(
				&(this->InitConfig.id3d12_descriptor_heap_description),
				__uuidof(ID3D12DescriptorHeap),
				(void**) & (this->Config.ptr_id3d12descriptorheap_v0)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12descriptorheap_v0->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12DescriptorHeap* WD3D12DescriptorHeap0::GetInterface
	()
	{
		return this->Config.ptr_id3d12descriptorheap_v0;
	}

	WD3D12DescriptorHeap0::~WD3D12DescriptorHeap0
	()
	{
		if (this->Config.ptr_id3d12descriptorheap_v0 != nullptr)
		{
			this->Config.ptr_id3d12descriptorheap_v0->Release();
			this->Config.ptr_id3d12descriptorheap_v0 = nullptr;
		}
	}





	// WD3D12Heap0 class.
	WD3D12Heap0::WD3D12Heap0
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12heap_v0 = nullptr;
		this->Config.unicode_name = L"WD3D12Heap0";
		this->Config.unicode_error_message = L"WD3D12Heap0.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"Heap0_01";

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		this->InitConfig.d3d12_heap_description.SizeInBytes = 0;
		this->InitConfig.d3d12_heap_description.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		this->InitConfig.d3d12_heap_description.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		this->InitConfig.d3d12_heap_description.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		this->InitConfig.d3d12_heap_description.Properties.CreationNodeMask = 0;
		this->InitConfig.d3d12_heap_description.Properties.VisibleNodeMask = 0;
		this->InitConfig.d3d12_heap_description.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		this->InitConfig.d3d12_heap_description.Flags = D3D12_HEAP_FLAG_NONE;
	}

	void WD3D12Heap0::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreateHeap
			(
				&(this->InitConfig.d3d12_heap_description),
				__uuidof(ID3D12Heap),
				(void**) & (this->Config.ptr_id3d12heap_v0)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12heap_v0->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12Heap* WD3D12Heap0::GetInterface
	()
	{
		return this->Config.ptr_id3d12heap_v0;
	}

	void WD3D12Heap0::ReleaseManually
	()
	{
		if (this->Config.ptr_id3d12heap_v0 != nullptr)
		{
			this->Config.ptr_id3d12heap_v0->Release();
			this->Config.ptr_id3d12heap_v0 = nullptr;
		}
	}

	WD3D12Heap0::~WD3D12Heap0
	()
	{
		if (this->Config.ptr_id3d12heap_v0 != nullptr)
		{
			this->Config.ptr_id3d12heap_v0->Release();
			this->Config.ptr_id3d12heap_v0 = nullptr;
		}
	}





	// WD3D12PlacedResource0 class.
	WD3D12PlacedResource0::WD3D12PlacedResource0
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12resource_v0 = nullptr;
		this->Config.unicode_name = L"WD3D12PlacedResource0";
		this->Config.unicode_error_message = L"WD3D12PlacedResource0.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"PlacedResource0_01";

		this->InitConfig.ptr_id3d12device_v5 = nullptr;
		this->InitConfig.ptr_id3d12heap_v0 = nullptr;

		this->InitConfig.heap_offset_in_bytes = 0;

		this->InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		this->InitConfig.d3d12_resource_description.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		this->InitConfig.d3d12_resource_description.Width = 0;
		this->InitConfig.d3d12_resource_description.Height = 1;
		this->InitConfig.d3d12_resource_description.DepthOrArraySize = 1;
		this->InitConfig.d3d12_resource_description.MipLevels = 1;
		this->InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
		this->InitConfig.d3d12_resource_description.SampleDesc.Count = 1;
		this->InitConfig.d3d12_resource_description.SampleDesc.Quality = 0;
		this->InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		this->InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_NONE;
		
		this->InitConfig.initial_resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	void WD3D12PlacedResource0::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreatePlacedResource
			(
				this->InitConfig.ptr_id3d12heap_v0,
				this->InitConfig.heap_offset_in_bytes,
				&(this->InitConfig.d3d12_resource_description),
				this->InitConfig.initial_resource_state,
				NULL,
				__uuidof(ID3D12Resource),
				(void**) & (this->Config.ptr_id3d12resource_v0)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3d12heap_v0 = nullptr;
		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12resource_v0->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12Resource* WD3D12PlacedResource0::GetInterface
	()
	{
		return this->Config.ptr_id3d12resource_v0;
	}

	void WD3D12PlacedResource0::ReleaseManually
	()
	{
		if (this->Config.ptr_id3d12resource_v0 != nullptr)
		{
			this->Config.ptr_id3d12resource_v0->Release();
			this->Config.ptr_id3d12resource_v0 = nullptr;
		}
	}

	WD3D12PlacedResource0::~WD3D12PlacedResource0
	()
	{
		if (this->Config.ptr_id3d12resource_v0 != nullptr)
		{
			this->Config.ptr_id3d12resource_v0->Release();
			this->Config.ptr_id3d12resource_v0 = nullptr;
		}
	}





	// WD3D12CommittedResource0 class.
	WD3D12CommittedResource0::WD3D12CommittedResource0
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12resource_v0 = nullptr;
		this->Config.unicode_name = L"WD3D12CommittedResource0";
		this->Config.unicode_error_message = L"WD3D12CommittedResource0.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"CommittedResource0_01";
		
		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		this->InitConfig.d3d12_heap_properties.Type = D3D12_HEAP_TYPE_CUSTOM;
		this->InitConfig.d3d12_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
		this->InitConfig.d3d12_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;
		this->InitConfig.d3d12_heap_properties.CreationNodeMask = 0;
		this->InitConfig.d3d12_heap_properties.VisibleNodeMask = 0;

		this->InitConfig.d3d12_heap_flags = D3D12_HEAP_FLAG_NONE;

		this->InitConfig.d3d12_resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		this->InitConfig.d3d12_resource_description.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		this->InitConfig.d3d12_resource_description.Width = 0;
		this->InitConfig.d3d12_resource_description.Height = 1;
		this->InitConfig.d3d12_resource_description.DepthOrArraySize = 1;
		this->InitConfig.d3d12_resource_description.MipLevels = 1;
		this->InitConfig.d3d12_resource_description.Format = DXGI_FORMAT_UNKNOWN;
		this->InitConfig.d3d12_resource_description.SampleDesc.Count = 1;
		this->InitConfig.d3d12_resource_description.SampleDesc.Quality = 0;
		this->InitConfig.d3d12_resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		this->InitConfig.d3d12_resource_description.Flags = D3D12_RESOURCE_FLAG_NONE;

		this->InitConfig.d3d12_resource_state = D3D12_RESOURCE_STATE_COMMON;	
	}

	void WD3D12CommittedResource0::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreateCommittedResource
			(
				&(this->InitConfig.d3d12_heap_properties),
				this->InitConfig.d3d12_heap_flags,
				&(this->InitConfig.d3d12_resource_description),
				this->InitConfig.d3d12_resource_state,
				NULL,
				__uuidof(ID3D12Resource),
				(void**) & (this->Config.ptr_id3d12resource_v0)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12resource_v0->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12Resource* WD3D12CommittedResource0::GetInterface
	()
	{
		return this->Config.ptr_id3d12resource_v0;
	}

	void WD3D12CommittedResource0::ReleaseManually
	()
	{
		if (this->Config.ptr_id3d12resource_v0 != nullptr)
		{
			this->Config.ptr_id3d12resource_v0->Release();
			this->Config.ptr_id3d12resource_v0 = nullptr;
		}
	}

	WD3D12CommittedResource0::~WD3D12CommittedResource0
	()
	{
		if (this->Config.ptr_id3d12resource_v0 != nullptr)
		{
			this->Config.ptr_id3d12resource_v0->Release();
			this->Config.ptr_id3d12resource_v0 = nullptr;
		}
	}





	// WD3D12RootSignature0 class.
	WD3D12RootSignature0::WD3D12RootSignature0
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12rootsignature_v0 = nullptr;
		this->Config.unicode_name = L"WD3D12RootSignature0";
		this->Config.unicode_error_message = L"WD3D12RootSignature0.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"RootSignature0_01";

		this->InitConfig.ptr_id3d12device_v5 = nullptr;
		this->InitConfig.ptr_id3dblob_v0 = nullptr;

		this->InitConfig.node_mask = 0;
	}

	void WD3D12RootSignature0::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreateRootSignature
			(
				this->InitConfig.node_mask,
				this->InitConfig.ptr_id3dblob_v0->GetBufferPointer(),
				this->InitConfig.ptr_id3dblob_v0->GetBufferSize(),
				__uuidof(ID3D12RootSignature),
				(void**) & (this->Config.ptr_id3d12rootsignature_v0)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3dblob_v0 = nullptr;
		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12rootsignature_v0->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12RootSignature* WD3D12RootSignature0::GetInterface
	()
	{
		return this->Config.ptr_id3d12rootsignature_v0;
	}

	WD3D12RootSignature0::~WD3D12RootSignature0
	()
	{
		if (this->Config.ptr_id3d12rootsignature_v0 != nullptr)
		{
			this->Config.ptr_id3d12rootsignature_v0->Release();
			this->Config.ptr_id3d12rootsignature_v0 = nullptr;
		}
	}
	




	// WD3D12StateObject class.
	WD3D12StateObject0::WD3D12StateObject0
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_id3d12stateobject_v0 = nullptr;
		this->Config.unicode_name = L"WD3D12StateObject0";
		this->Config.unicode_error_message = L"WD3D12StateObject0.Initialize() failed.";

		this->InitConfig.unicode_debug_name = L"StateObject0_01";

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		this->InitConfig.d3d12_state_object_description.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
		this->InitConfig.d3d12_state_object_description.NumSubobjects = 0;
		this->InitConfig.d3d12_state_object_description.pSubobjects = nullptr;
	}

	void WD3D12StateObject0::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_id3d12device_v5->CreateStateObject
			(
				&(this->InitConfig.d3d12_state_object_description),
				__uuidof(ID3D12StateObject),
				(void**) & (this->Config.ptr_id3d12stateobject_v0)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		this->InitConfig.ptr_id3d12device_v5 = nullptr;

		FailCheck
		(
			this->Config.ptr_id3d12stateobject_v0->SetName
			(
				this->InitConfig.unicode_debug_name
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	ID3D12StateObject* WD3D12StateObject0::GetInterface
	()
	{
		return this->Config.ptr_id3d12stateobject_v0;
	}

	WD3D12StateObject0::~WD3D12StateObject0
	()
	{
		if (this->Config.ptr_id3d12stateobject_v0 != nullptr)
		{
			this->Config.ptr_id3d12stateobject_v0->Release();
			this->Config.ptr_id3d12stateobject_v0 = nullptr;
		}
	}

}
