// Win32Window.hpp (Header-Only) - Basic Win32 Window class.
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef WIN_32_LEAN_AND_MEAN
#define WIN_32_LEAN_AND_MEAN
#endif

#include <Windows.h>

// Config data for this class.
struct Win32WindowConfig
{
	// Window class properties.
	WNDCLASSEX WindowClassExtended;

	// Window creation values.
	const wchar_t* UnicodeWindowName;
	DWORD WindowStyle;
	int WindowOffsetX;
	int WindowOffsetY;
	unsigned int WidthCorrection;
	unsigned int HeightCorrection;
	RECT WindowRectangle;

	// The Win32 handle to the created window.
	HWND WindowHandle;
};

// Populate this before calling the initializer function.
struct Win32WindowInitConfig
{
	// Desired dimensions of the client area.
	unsigned int Width;
	unsigned int Height;
};

// Basic and lean Win32 window.
class Win32Window
{
public:
	Win32Window() :	InitConfig{}, Config{}
	{
		this->Config.WindowClassExtended.cbSize = sizeof(WNDCLASSEX);
		this->Config.WindowClassExtended.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		this->Config.WindowClassExtended.hCursor = LoadCursorW(NULL, IDC_ARROW);
		this->Config.WindowClassExtended.hInstance = GetModuleHandleW(NULL);
		this->Config.WindowClassExtended.lpfnWndProc = this->WindowProcedure;
		this->Config.WindowClassExtended.lpszClassName = L"WindowCanvas";
		this->Config.WindowClassExtended.style = CS_HREDRAW | CS_VREDRAW;

		this->Config.UnicodeWindowName = L"Should_not_see";
		this->Config.WindowStyle = WS_VISIBLE;
		this->Config.WindowOffsetX = -8;
		this->Config.WindowOffsetY = -32;

		this->Config.WidthCorrection = 16U;
		this->Config.HeightCorrection = 40U;

		this->Config.WindowHandle = NULL;

		this->InitConfig.Width = 0U;
		this->InitConfig.Height = 0U;
	}

	// Populate this before calling the initializer function.
	Win32WindowInitConfig InitConfig;

	// Initializes the instance of this class.
	void Initialize()
	{
		// Register the window class.
		if (RegisterClassExW(&(this->Config.WindowClassExtended)) == 0)
		{
			MessageBoxW
			(
				NULL,
				L"RegisterClassEx() failed.",
				L"Win32Window Error",
				NULL
			);
		}

		// Prepare values and create the window.
		this->Config.WindowRectangle.left = (this->InitConfig.Width) - (this->InitConfig.Width);
		this->Config.WindowRectangle.right = (this->InitConfig.Width) + (this->Config.WidthCorrection);
		this->Config.WindowRectangle.top = (this->InitConfig.Height) - (this->InitConfig.Height);
		this->Config.WindowRectangle.bottom = (this->InitConfig.Height) + (this->Config.HeightCorrection);

		this->Config.WindowHandle = CreateWindowExW(
			NULL,
			this->Config.WindowClassExtended.lpszClassName,
			this->Config.UnicodeWindowName,
			this->Config.WindowStyle,
			this->Config.WindowOffsetX,
			this->Config.WindowOffsetY,
			this->Config.WindowRectangle.right - this->Config.WindowRectangle.left,
			this->Config.WindowRectangle.bottom - this->Config.WindowRectangle.top,
			NULL,
			NULL,
			GetModuleHandleW(NULL),
			NULL
		);

		if (this->Config.WindowHandle == NULL)
		{
			MessageBoxW(
				NULL,
				L"CreateWindowEx() failed.",
				L"Win32Window Error",
				NULL
			);
		}

		SetWindowPos(
			this->Config.WindowHandle,
			HWND_TOP,
			this->Config.WindowOffsetX,
			this->Config.WindowOffsetY,
			this->Config.WindowRectangle.right - this->Config.WindowRectangle.left,
			this->Config.WindowRectangle.bottom - this->Config.WindowRectangle.top,
			SWP_NOSENDCHANGING
		);
	}

	// Returns the handle to the window.
	HWND GetWindowHandle()
	{
		return this->Config.WindowHandle;
	}

	~Win32Window()
	{
		// Nothing here, for now.
	}

protected:
	// Config data for this object.
	Win32WindowConfig Config;

	// Window procedure function.
	static LRESULT CALLBACK WindowProcedure(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	)
	{
		return DefWindowProcW(
			hWnd,
			msg,
			wParam,
			lParam
		);
	}

};
