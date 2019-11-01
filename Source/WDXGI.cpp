// WDXGI.cpp - Wrappers for some of the DirectX Graphics Infrastructure interfaces.
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#include "WDXGI.hpp"

namespace WDXGI
{
	// WDXGIDebug1 class.
	WDXGIDebug1::WDXGIDebug1
	()
	{
		this->Config.ptr_idxgidebug_v1 = nullptr;
		this->Config.unicode_name = L"WDXGIDebug1";
		this->Config.unicode_error_message = L"WDXGIDebug1.Initialize() failed.";

		this->InitConfig.EnableDebugLayer = false;
		this->InitConfig.dxgi_debug_id = DXGI_DEBUG_ALL;
		this->InitConfig.dxgi_debug_rlo_flags = DXGI_DEBUG_RLO_ALL;
	}

	void WDXGIDebug1::Initialize
	()
	{
		FailCheck
		(
			DXGIGetDebugInterface1
			(
				NULL,
				__uuidof(IDXGIDebug1),
				(void**) & (this->Config.ptr_idxgidebug_v1)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);

		if (this->InitConfig.EnableDebugLayer == true)
		{
			FailCheck
			(
				this->Config.ptr_idxgidebug_v1->ReportLiveObjects
				(
					this->InitConfig.dxgi_debug_id,
					this->InitConfig.dxgi_debug_rlo_flags
				),
				this->Config.unicode_error_message,
				this->Config.unicode_name
			);			
		}
	}

	IDXGIDebug1* WDXGIDebug1::GetInterface
	()
	{
		return this->Config.ptr_idxgidebug_v1;
	}

	WDXGIDebug1::~WDXGIDebug1
	()
	{
		if (this->Config.ptr_idxgidebug_v1 != nullptr)
		{
			this->Config.ptr_idxgidebug_v1->Release();
			this->Config.ptr_idxgidebug_v1 = nullptr;
		}		
	}





	// WD3D12Factory7 class.
	WDXGIFactory7::WDXGIFactory7
	() :
		Config{}
	{
		this->Config.ptr_idxgifactory_v7 = nullptr;
		this->Config.unicode_name = L"WDXGIFactory7";
		this->Config.unicode_error_message = L"WDXGIFactory7.Initialize() failed.";
	}

	void WDXGIFactory7::Initialize
	()
	{
		FailCheck
		(
			CreateDXGIFactory
			(
				__uuidof(IDXGIFactory7),
				(void**) & (this->Config.ptr_idxgifactory_v7)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name
		);
	}

	IDXGIFactory7* WDXGIFactory7::GetInterface
	()
	{
		return this->Config.ptr_idxgifactory_v7;
	}

	WDXGIFactory7::~WDXGIFactory7
	()
	{
		if (this->Config.ptr_idxgifactory_v7 != nullptr)
		{
			this->Config.ptr_idxgifactory_v7->Release();
			this->Config.ptr_idxgifactory_v7 = nullptr;
		}		
	}




	
	// WDXGISwapChain4 class.
	WDXGISwapChain4::WDXGISwapChain4
	() :
		InitConfig{},
		Config{}
	{
		this->Config.ptr_idxgiswapchain_v4 = nullptr;
		this->Config.unicode_name = L"WDXGISwapChain4";
		this->Config.unicode_error_message = L"WDXGISwapChain4.Initialize() failed.";

		this->InitConfig.ptr_idxgifactory_v7 = nullptr;
		this->InitConfig.ptr_id3d12commandqueue_v0 = nullptr;

		this->InitConfig.win32_window_handle = NULL;

		this->InitConfig.dxgi_swap_chain_description_v1.Width = 0;
		this->InitConfig.dxgi_swap_chain_description_v1.Height = 0;
		this->InitConfig.dxgi_swap_chain_description_v1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		this->InitConfig.dxgi_swap_chain_description_v1.Stereo = FALSE;
		this->InitConfig.dxgi_swap_chain_description_v1.SampleDesc.Count = 1;
		this->InitConfig.dxgi_swap_chain_description_v1.SampleDesc.Quality = 0;
		this->InitConfig.dxgi_swap_chain_description_v1.BufferUsage = NULL;
		this->InitConfig.dxgi_swap_chain_description_v1.BufferCount = 2;
		this->InitConfig.dxgi_swap_chain_description_v1.Scaling = DXGI_SCALING_NONE;
		this->InitConfig.dxgi_swap_chain_description_v1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		this->InitConfig.dxgi_swap_chain_description_v1.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		this->InitConfig.dxgi_swap_chain_description_v1.Flags = NULL;		
	}

	void WDXGISwapChain4::Initialize
	()
	{
		FailCheck
		(
			this->InitConfig.ptr_idxgifactory_v7->CreateSwapChainForHwnd
			(
				this->InitConfig.ptr_id3d12commandqueue_v0,
				this->InitConfig.win32_window_handle,
				&(this->InitConfig.dxgi_swap_chain_description_v1),
				NULL,
				NULL,
				(IDXGISwapChain1**) & (this->Config.ptr_idxgiswapchain_v4)
			),
			this->Config.unicode_error_message,
			this->Config.unicode_name			
		);

		this->InitConfig.ptr_id3d12commandqueue_v0 = nullptr;
		this->InitConfig.ptr_idxgifactory_v7 = nullptr;
	}

	IDXGISwapChain4* WDXGISwapChain4::GetInterface
	()
	{
		return this->Config.ptr_idxgiswapchain_v4;
	}

	WDXGISwapChain4::~WDXGISwapChain4
	()
	{
		if (this->Config.ptr_idxgiswapchain_v4 != nullptr)
		{
			this->Config.ptr_idxgiswapchain_v4->Release();
			this->Config.ptr_idxgiswapchain_v4 = nullptr;
		}		
	}

}
