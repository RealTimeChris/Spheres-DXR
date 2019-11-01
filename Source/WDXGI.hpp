// WDXGI.cpp - Wrappers for some of the DirectX Graphics Infrastructure interfaces.
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "dxgi1_6.h"
#pragma comment(lib, "dxgi.lib")

#include "dxgidebug.h"
#pragma comment(lib, "dxguid.lib")

#include "d3d12.h"
#pragma comment(lib, "d3d12.lib")

// Wrappers for some of the DirectX Graphics Infrastructure interfaces.
namespace WDXGI
{
	// Utility function.
	inline void FailCheck
	(
		HRESULT			hResult,
		const wchar_t*	unicode_error_message,
		const wchar_t*	unicode_error_title
	)
	{
		if (hResult != S_OK)
		{
			MessageBoxW
			(
				NULL,
				unicode_error_message,
				unicode_error_title,
				NULL
			);
		}
	}





	// Config data for this class.
	struct WDXGIDebug1Config
	{
		// Pointer to the contained interface.
		IDXGIDebug1* ptr_idxgidebug_v1;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WDXGIDebug1InitConfig
	{
		// Enable the DXGI debug layer?
		bool EnableDebugLayer;

		// Identifier to specify which objects to get debug info about.
		GUID dxgi_debug_id;

		// Specifies the amount of info to report.
		DXGI_DEBUG_RLO_FLAGS dxgi_debug_rlo_flags;
	};

	// Wrapper class for the IDXGIDebug1 interface.
	class WDXGIDebug1
	{
	public:
		// Constructor.
		WDXGIDebug1();

		// Populate this before calling the initializer function.
		WDXGIDebug1InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		IDXGIDebug1* GetInterface();

		// Destructor.
		~WDXGIDebug1();

	protected:
		// Config data for this object.
		WDXGIDebug1Config Config;

	};





	// Config data for this class.
	struct WDXGIFactory7Config
	{
		// Pointer to the contained interface.
		IDXGIFactory7* ptr_idxgifactory_v7;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Wrapper class for the IDXGIFactory7 interface.
	class WDXGIFactory7
	{
	public:
		// Constructor.
		WDXGIFactory7();

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		IDXGIFactory7* GetInterface();

		// Destructor.
		~WDXGIFactory7();

	protected:
		// Config data for this object.
		WDXGIFactory7Config Config;

	};





	// Config data for this class.
	struct WDXGISwapChain4Config
	{
		// Pointer to the contained interface.
		IDXGISwapChain4* ptr_idxgiswapchain_v4;

		// Label for this class.
		const wchar_t* unicode_name;

		// Error message for initialization.
		const wchar_t* unicode_error_message;
	};

	// Populate this before calling the initializer function.
	struct WDXGISwapChain4InitConfig
	{
		// Pointer to a factory interface, for creation.
		IDXGIFactory7* ptr_idxgifactory_v7;

		// Pointer to a direct command queue interface, for creation.
		ID3D12CommandQueue* ptr_id3d12commandqueue_v0;

		// Handle to a Win32 window.
		HWND win32_window_handle;

		// Description of the desired swap chain.
		DXGI_SWAP_CHAIN_DESC1 dxgi_swap_chain_description_v1;
	};

	// Wrapper class for the IDXGISwapChain4 interface.
	class WDXGISwapChain4
	{
	public:
		// Constructor.
		WDXGISwapChain4();

		// Populate this before calling the initializer function.
		WDXGISwapChain4InitConfig InitConfig;

		// Initializes the instance of this class.
		void Initialize();

		// Returns a pointer to the contained interface.
		IDXGISwapChain4* GetInterface();

		// Destructor.
		~WDXGISwapChain4();

	protected:
		// Config data for this object.
		WDXGISwapChain4Config Config;

	};

}
