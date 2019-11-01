// RGBAWelcomeMat.hpp - For generating and supplying "Hello World" arrays of RGB/RGBA data.
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

// Calculates the delta-per-pixel for a given color channel, based on min, max, and pixel count values.
// NOTE: To be used with (0.0f to 1.0f) normalized float color values.
inline void ColorDeltaPerPixel
(
	float*			p_color_delta_per_pixel,
	float			color_val_zero,
	float			color_val_n,
	unsigned int	pixel_count
)
{
	if (color_val_n >= color_val_zero)
	{
		*p_color_delta_per_pixel = (color_val_n - color_val_zero) / (float)pixel_count;
	}

	else if (color_val_zero >= color_val_n)
	{
		*p_color_delta_per_pixel = (color_val_zero - color_val_n) / (float)pixel_count;
	}
}

// Calculates the color value of a single channel for a given pixel, given some delta and reference values.
// NOTE: Converts from float32 normalized values to uint8 integer values. (value * 255)
inline void ColorValCurrentPixel
(
	unsigned char*	p_color_val_current,
	float			color_delta_per_pixel,
	float			color_val_zero,
	float			color_val_n,
	unsigned int	pixel_index
)
{
	// Ascending
	if (color_val_n >= color_val_zero)
	{
		*p_color_val_current = (unsigned char)((color_val_zero + (color_delta_per_pixel * pixel_index)) * 255.0f);
	}
	// Descending
	else if (color_val_zero >= color_val_n)
	{
		*p_color_val_current = (unsigned char)((color_val_zero - (color_delta_per_pixel * pixel_index)) * 255.0F);
	}

	// Clip the value to the correct range
	if ((*p_color_val_current) >= 255)
	{
		*p_color_val_current = 255U;
	}
	else if ((*p_color_val_current) <= 0)
	{
		*p_color_val_current = 0U;
	}
}

// Enumeration for selecting which color format to use for color data/"Welcome Mat" generation.
enum COLOR_FORMAT
{
	COLOR_FORMAT_B8G8R8 = 0,
	COLOR_FORMAT_R8G8B8A8 = 1
};

// Config data for this class.
struct RGBAWelcomeMatConfig
{
	// For calculating and allocating the array to be used for storing the color data.
	unsigned int pixel_count;
	unsigned int bytes_per_pixel;
	unsigned int array_byte_size;

	// The RGBA/RGB array, to be dynamically allocated.
	// NOTE: UNORM = unsigned int in the resource, but 0.0f to 1.0f in the shaders.
	unsigned char* color_data_array;

	// Start and end values for each color channel, across the array.
	float r_zero, r_n;
	float g_zero, g_n;
	float b_zero, b_n;
	
	// Delta values for calculating each pixel's color channels.
	float delta_r_per_pixel;
	float delta_g_per_pixel;
	float delta_b_per_pixel;
	// No need for delta_a_per_pixel. Use a_current, which is set to 0 in the constructor.

	// Values for generating and storing the color data.
	unsigned char r_current;
	unsigned char g_current;
	unsigned char b_current;
	unsigned char a_current;

	unsigned int r_byte_offset;
	unsigned int g_byte_offset;
	unsigned int b_byte_offset;
	unsigned int a_byte_offset;

	unsigned int index;

	// For checking the success of the memcpy_s() call inside RGBAWelcomeMat.Greet()
	// memcpy_s: Returns zero if succesful, otherwise returns an error code of some kind.
	errno_t	result;
};

// Populate this before calling the initializer function.
struct RGBAWelcomeMatInitConfig
{
	// Desired number of pixels along each dimension.
	unsigned int pixel_height;
	unsigned int pixel_width;

	// Selects which color format to use for generating the color data/welcome mat.
	COLOR_FORMAT color_format;
};

// Class for generating and supplying "Hello World" arrays of RGB / RGBA data.
// NOTE: Set the desired color format using the InitConfig struct.
class RGBAWelcomeMat
{
public:
	// Constructor.
	RGBAWelcomeMat();

	// Populate this before calling the initializer function.
	RGBAWelcomeMatInitConfig InitConfig;

	// Initializes the instance of this class.
	// Generates the color data and stores it in a dynamically allocated array.
	void Initialize();

	// Copies all of the color data (the "Welcome Mat") to a memory location starting with the provided address.
	// NOTE: Be sure that you have control of the destination memory and know that the data belongs there.
	void Greet
	(
		void* color_data_destination_address
	);

	// Destructor.
	~RGBAWelcomeMat();

protected:
	// Config data for this object.
	RGBAWelcomeMatConfig Config;

	// Generates the appropriate array of color data.	
	void GenerateColorData();

};
