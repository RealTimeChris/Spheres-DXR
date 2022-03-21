// WD3D12.hpp - Wrappers for some of the core Direct3D 12 interfaces.
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <d3d12sdklayers.h>

// Wrappers for some of the core Direct3D 12 interfaces.
namespace WD3D12
{
	// Utility function.
	inline void FailCheck(
		HRESULT	hResult,
		const wchar_t* unicode_error_message,
		const wchar_t* unicode_error_title
	)
	{
		if (hResult != S_OK)
		{
			MessageBoxW(
				NULL,
				unicode_error_message,
				unicode_error_title,
				NULL
			);
		}
	}





	// Config data for this class.
	struct WD3D12Debug0Config
	{
		// Pointer to the contained interface.
		ID3D12Debug* ptr_id3d12debug_v0;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12Debug0InitConfig
	{
		// Enable the debug layer?
		bool enable_debug_layer;
	};

	// Wrapper class for the ID3D12Debug interface.
	class WD3D12Debug0
	{
	public:
		// Constructor.
		WD3D12Debug0();

		// Populate this before calling the initializer function.
		WD3D12Debug0InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12Debug* GetInterface();

		// Destructor.
		~WD3D12Debug0();

	protected:
		// Config data for this object.
		WD3D12Debug0Config Config;

	};





	// Config data for this class.
	struct WD3D12Device5Config
	{
		// Pointer to the contained interface.
		ID3D12Device5* ptr_id3d12device_v5;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12Device5InitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to an IDXGI adapter. Use NULL for default adapter on the system.
		IUnknown* ptr_adapter;

		// Hardware feature level requested for the virtual device.
		D3D_FEATURE_LEVEL	d3d_feature_level;
	};

	// Wrapper class for the ID3D12Device5 interface.
	class WD3D12Device5
	{
	public:
		// Constructor.
		WD3D12Device5();

		// Populate this before calling the initializer function.
		WD3D12Device5InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12Device5* GetInterface();

		// Destructor
		~WD3D12Device5();

	protected:
		// Config data for this object.
		WD3D12Device5Config Config;

	};





	// Configuration data for this class.
	struct WD3D12CommandQueue0Config
	{
		// Pointer to the contained interface.
		ID3D12CommandQueue* ptr_id3d12commandqueue_v0;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this structure before calling the initializer function.
	struct WD3D12CommandQueue0InitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;

		// Description of the desired interface.
		D3D12_COMMAND_QUEUE_DESC d3d12_command_queue_desc;
	};

	// Wrapper class for the ID3D12CommandQueue interface.
	class WD3D12CommandQueue0
	{
	public:
		// Constructor.
		WD3D12CommandQueue0();

		// Populate this structure before calling the initializer function.
		WD3D12CommandQueue0InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12CommandQueue* GetInterface();

		// Destructor.
		~WD3D12CommandQueue0();

	protected:
		// Config data for this object.
		WD3D12CommandQueue0Config Config;

	};





	// Config data for this class.
	struct WD3D12CommandAllocator0Config
	{
		// Pointer to the contained interface.
		ID3D12CommandAllocator* ptr_id3d12commandallocator_v0;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12CommandAllocator0InitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;

		// Specifies which type of commands to allocate space for.
		D3D12_COMMAND_LIST_TYPE	d3d12_command_list_type;
	};

	// Wrapper class for the ID3D12CommandAllocator interface.
	class WD3D12CommandAllocator0
	{
	public:
		// Constructor.
		WD3D12CommandAllocator0();

		// Populate this before calling the initializer function.
		WD3D12CommandAllocator0InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12CommandAllocator* GetInterface();

		// Destructor.
		~WD3D12CommandAllocator0();

	protected:
		// Config data for this object.
		WD3D12CommandAllocator0Config Config;

	};





	// Config data for this class.
	struct WD3D12GraphicsCommandList4Config
	{
		// Pointer to the contained interface.
		ID3D12GraphicsCommandList4* ptr_id3d12graphicscommandlist_v4;

		// Label for this class.
		const wchar_t* unicode_name;

		// Erorr mesage for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12GraphicsCommandList4InitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;

		// Set to 0 for single/default GPU.
		UINT node_mask;

		// Type of command list.
		D3D12_COMMAND_LIST_TYPE d3d12_command_list_type;

		// Pointer to a command allocator interface, for creation.
		ID3D12CommandAllocator* ptr_id3d12commandallocator_v0;

		// Pointer to a pipeline state interface, for creation. Set to NULL for no initial state.
		ID3D12PipelineState* ptr_id3d12pipelinestate_v0;
	};

	// Wrapper class for the ID3D12GraphicsCommandList4 interface.
	class WD3D12GraphicsCommandList4
	{
	public:
		// Constructor.
		WD3D12GraphicsCommandList4();

		// Populate this before calling the initializer function.
		WD3D12GraphicsCommandList4InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12GraphicsCommandList4* GetInterface();

		// Releases the contained interface now, instead of waiting for the destructor.
		void ReleaseManually();

		// Destructor.
		~WD3D12GraphicsCommandList4();

	protected:
		// Config data for this object.
		WD3D12GraphicsCommandList4Config Config;

	};





	// Config data for this class.
	struct WD3D12Fence1Config
	{
		// Pointer to the contained interface.
		ID3D12Fence1* ptr_id3d12fence_v1;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12Fence1InitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;

		// Initial value for the fence.
		UINT64 fence_initial_value;

		// Fence flags for creation.
		D3D12_FENCE_FLAGS d3d12_fence_flags;
	};

	// Wrapper class for the ID3D12Fence1 interface.
	class WD3D12Fence1
	{
	public:
		// Constructor.
		WD3D12Fence1();

		// Populate this before calling the initializer function.
		WD3D12Fence1InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12Fence1* GetInterface();

		// Releases the contained interface now, instead of waiting for the destructor.
		void ReleaseManually();

		// Destructor.
		~WD3D12Fence1();

	protected:
		// Config data for this object.
		WD3D12Fence1Config Config;

	};





	// Config data for this class.
	struct WD3D12DescriptorHeap0Config
	{
		// Pointer to the contained interface.
		ID3D12DescriptorHeap* ptr_id3d12descriptorheap_v0;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12DescriptorHeap0InitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;

		// Description of the desired descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC id3d12_descriptor_heap_description;
	};

	// Wrapper class for the ID3D12DescriptorHeap interface.
	class WD3D12DescriptorHeap0
	{
	public:
		// Constructor.
		WD3D12DescriptorHeap0();

		// Populate this before calling the initializer function.
		WD3D12DescriptorHeap0InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12DescriptorHeap* GetInterface();

		// Destructor.
		~WD3D12DescriptorHeap0();

	protected:
		// Config data for this object.
		WD3D12DescriptorHeap0Config Config;

	};





	// Config data for this class.
	struct WD3D12Heap0Config
	{
		// Pointer to the contained interface.
		ID3D12Heap* ptr_id3d12heap_v0;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12Heap0InitConfig
	{
		// For the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;

		// Description of the desired heap.
		D3D12_HEAP_DESC	d3d12_heap_description;
	};

	// Wrapper class for the ID3D12Heap interface
	class WD3D12Heap0
	{
	public:
		// Constructor.
		WD3D12Heap0();

		// Populate this before calling the initializer function.
		WD3D12Heap0InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface
		ID3D12Heap* GetInterface();

		// Releases the contained interface now, instead of waiting for the destructor.
		void ReleaseManually();

		// Destructor.
		~WD3D12Heap0();

	protected:
		// Config data for this object.
		WD3D12Heap0Config Config;

	};





	// Config data for this class.
	struct WD3D12PlacedResource0Config
	{
		// Pointer to the contained interface.
		ID3D12Resource* ptr_id3d12resource_v0;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12PlacedResource0InitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;

		// Pointer to a heap interface, for creation.
		ID3D12Heap* ptr_id3d12heap_v0;

		// Desired offset in bytes, within the target heap, to place the resource.
		// NOTE: Offset must be a multiple of the target heap's alignment.
		UINT64 heap_offset_in_bytes;

		// Description of the desired resource.
		D3D12_RESOURCE_DESC d3d12_resource_description;

		// Initial resource state. As a bitwise-Or'd combination.
		D3D12_RESOURCE_STATES initial_resource_state;
	};

	// Wrapper class for the ID3D12Resource interface, in the context of Placed Resources.
	class WD3D12PlacedResource0
	{
	public:
		// Constructor.
		WD3D12PlacedResource0();

		// Populate this before calling the initiailizer function.
		WD3D12PlacedResource0InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12Resource* GetInterface();

		// Releases the contained interface now, instead of waiting for the destructor.
		void ReleaseManually();

		// Destructor.
		~WD3D12PlacedResource0();

	protected:
		// Config data for this object.
		WD3D12PlacedResource0Config Config;

	};





	// Config data for this class.
	struct WD3D12CommittedResourec0Config
	{
		// Pointer to the contained interface.
		ID3D12Resource* ptr_id3d12resource_v0;

		// Label for this class,
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12CommittedResource0InitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;

		// Desired properties of the committed resource's heap.
		D3D12_HEAP_PROPERTIES d3d12_heap_properties;

		// Desired flags of the committed resource's heap.
		D3D12_HEAP_FLAGS d3d12_heap_flags;

		// Description of the committed resource.
		D3D12_RESOURCE_DESC d3d12_resource_description;

		// Initial state of the committed resource.
		D3D12_RESOURCE_STATES d3d12_resource_state;

		// Default value for a clear color. Set to NULL for buffers.
		//D3D12_CLEAR_VALUE d3d12_clear_value;
	};

	// Wrapper class for the ID3D12Resource interface, in the context of Committed Resources.
	class WD3D12CommittedResource0
	{
	public:
		// Constructor.
		WD3D12CommittedResource0();

		// Populate this before calling the initializer function.
		WD3D12CommittedResource0InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12Resource* GetInterface();

		// Releases the contained interface now, instead of waiting for the destructor.
		void ReleaseManually();

		// Destructor.
		~WD3D12CommittedResource0();

	protected:
		// Config data for this object.
		WD3D12CommittedResourec0Config Config;
		
	};





	// Config data for this class.
	struct WD3D12RootSignature0Config
	{
		// Pointer to the contained interface.
		ID3D12RootSignature* ptr_id3d12rootsignature_v0;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12RootSignature0InitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;
		
		// Pointer to a blob interface containing the serialized root signature.
		ID3DBlob* ptr_id3dblob_v0;

		// Node mask, for selecting the GPU.
		UINT node_mask;
	};

	// Wrapper class for the ID3D12RootSignature interface.
	class WD3D12RootSignature0
	{
	public:
		// Constructor.
		WD3D12RootSignature0();

		// Populate this before calling the initializer function.
		WD3D12RootSignature0InitConfig InitConfig;

		// Initializes the instances of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12RootSignature* GetInterface();

		// Destructor.
		~WD3D12RootSignature0();

	protected:
		// Config data for this object.
		WD3D12RootSignature0Config Config;

	};





	// Config data for this class
	struct WD3D12StateObject0Config
	{
		// Pointer to the contained interface.
		ID3D12StateObject* ptr_id3d12stateobject_v0;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WD3D12StateObjectInitConfig
	{
		// Name for the debugging layer.
		const wchar_t* unicode_debug_name;

		// Pointer to a device interface, for creation.
		ID3D12Device5* ptr_id3d12device_v5;

		// Description of the State Object to be created.
		D3D12_STATE_OBJECT_DESC d3d12_state_object_description;
	};

	// Wrapper class for the ID3D12StateObject interface.
	class WD3D12StateObject0
	{
	public:
		// Constructor.
		WD3D12StateObject0();

		// Populate this before calling the initializer function.
		WD3D12StateObjectInitConfig InitConfig;

		// Initializes the instances of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		ID3D12StateObject* GetInterface();

		// Destructor.
		~WD3D12StateObject0();

	protected:
		// Config data for this object.
		WD3D12StateObject0Config Config;

	};

}
