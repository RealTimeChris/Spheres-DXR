// RGBAWelcomeMat.cpp - For generating and supplying "Hello World" arrays of RGB/RGBA data.
// October 2019
// Chris M.
// https://github.com/RealTimeChris

#include "RGBAWelcomeMat.hpp"

RGBAWelcomeMat::RGBAWelcomeMat
() :
	InitConfig{},
	Config{}
{
	this->Config.pixel_count = 0;
	this->Config.bytes_per_pixel = 0;
	this->Config.array_byte_size = 0;

	this->Config.color_data_array = nullptr;

	// Keep these values between 0.1f and 1.0f, then multiply by 255.0f to convert.
	this->Config.r_zero = 0.2f;
	this->Config.r_n = 0.95f;
	this->Config.g_zero = 1.0f;
	this->Config.g_n = 0.1f;
	this->Config.b_zero = 0.1f;
	this->Config.b_n = 1.0f;

	this->Config.delta_r_per_pixel = 0;
	this->Config.delta_g_per_pixel = 0;
	this->Config.delta_b_per_pixel = 0;
	
	// Values for generating and storing the color data.
	this->Config.r_current = 0;
	this->Config.g_current = 0;
	this->Config.b_current = 0;
	this->Config.a_current = 0;

	this->Config.r_byte_offset = 0;
	this->Config.g_byte_offset = 0;
	this->Config.b_byte_offset = 0;
	this->Config.a_byte_offset = 0;
	
	this->Config.index = 0;

	this->Config.result = 1;

	this->InitConfig.pixel_height = 0;
	this->InitConfig.pixel_width = 0;
	this->InitConfig.color_format = COLOR_FORMAT_R8G8B8A8;
}

void RGBAWelcomeMat::Initialize
()
{
	// Calculate pixel count.
	this->Config.pixel_count = (this->InitConfig.pixel_width) * (this->InitConfig.pixel_height);

	// Decide on bytes-per-pixel.
	switch (this->InitConfig.color_format)
	{
	case (COLOR_FORMAT_B8G8R8):
	{
		this->Config.bytes_per_pixel = 3;
	} break;
		
	case (COLOR_FORMAT_R8G8B8A8):
	{
		this->Config.bytes_per_pixel = 4;
	} break;
	}

	// Calculate required array size for the color data, in bytes.
	this->Config.array_byte_size = (this->Config.pixel_count) * (this->Config.bytes_per_pixel);

	// Allocate the array.
	this->Config.color_data_array = new unsigned char
		[
			(this->Config.array_byte_size)
		];

	// Calculate the color-delta-per-pixel for each color channel.
	ColorDeltaPerPixel
	(
		&(this->Config.delta_r_per_pixel),
		this->Config.r_zero,
		this->Config.r_n,
		this->Config.pixel_count
	);

	ColorDeltaPerPixel
	(
		&(this->Config.delta_g_per_pixel),
		this->Config.g_zero,
		this->Config.g_n,
		this->Config.pixel_count
	);

	ColorDeltaPerPixel
	(
		&(this->Config.delta_b_per_pixel),
		this->Config.b_zero,
		this->Config.b_n,
		this->Config.pixel_count
	);

	// Set the indexing-offsets for each color channel, depending on the selected color format.
	switch (this->InitConfig.color_format)
	{
	case (COLOR_FORMAT_B8G8R8):
	{
		this->Config.r_byte_offset = 2;
		this->Config.g_byte_offset = 1;
		this->Config.b_byte_offset = 0;
	} break;

	case (COLOR_FORMAT_R8G8B8A8):
	{
		this->Config.r_byte_offset = 0;
		this->Config.g_byte_offset = 1;
		this->Config.b_byte_offset = 2;
		this->Config.a_byte_offset = 3;
	} break;
	}

	// Generate the appropriate color data/Welcome Mat, depending on the selected color format.
	this->GenerateColorData();
}

void RGBAWelcomeMat::Greet
(
	void* color_data_destination_address
)
{
	// Reset the result value before copying.
	this->Config.result = 1;

	// Copy color data to the provided destination address.
	this->Config.result = memcpy_s
		(
			color_data_destination_address,
			this->Config.array_byte_size,
			this->Config.color_data_array,
			this->Config.array_byte_size
		);
	
	if (this->Config.result != 0)
	{
		MessageBox
		(
			NULL,
			L"Error: memcpy_s() unsuccesful.",
			L"RGBAWelcomeMat.Greet() Failure",
			NULL
		);
	}
}

RGBAWelcomeMat::~RGBAWelcomeMat
()
{
	delete[](this->Config.color_data_array);
	this->Config.color_data_array = nullptr;
}


/*
	Protected member functions.
*/

void RGBAWelcomeMat::GenerateColorData
()
{
	// Calculate current pixel values for each channel and populate the RGBA array.
	for ((this->Config.index) = 0; (this->Config.index) < (this->Config.pixel_count); (this->Config.index)++)
	{
				
		// Calculate and clip the values (Watch for conversions..)
		ColorValCurrentPixel
		(
			&(this->Config.b_current),
			this->Config.delta_b_per_pixel,
			this->Config.b_zero,
			this->Config.b_n,
			this->Config.index
		);
		
		ColorValCurrentPixel
		(
			&(this->Config.g_current),
			this->Config.delta_g_per_pixel,
			this->Config.g_zero,
			this->Config.g_n,
			this->Config.index
		);

		ColorValCurrentPixel
		(
			&(this->Config.r_current),
			this->Config.delta_r_per_pixel,
			this->Config.r_zero,
			this->Config.r_n,
			this->Config.index
		);
				
		// Store the values in the array.
		this->Config.color_data_array
			[
				(((this->Config.index) * (this->Config.bytes_per_pixel)) + (this->Config.b_byte_offset))
			]
		= this->Config.b_current;
		
		this->Config.color_data_array
			[
				(((this->Config.index) * (this->Config.bytes_per_pixel)) + (this->Config.g_byte_offset))
			]
		= this->Config.g_current;

		this->Config.color_data_array
			[
				(((this->Config.index) * (this->Config.bytes_per_pixel)) + (this->Config.r_byte_offset))
			]
		= this->Config.r_current;

		if (this->InitConfig.color_format == COLOR_FORMAT_R8G8B8A8)
		{
			this->Config.color_data_array
				[
					(((this->Config.index) * (this->Config.bytes_per_pixel)) + (this->Config.a_byte_offset))
				]
			= this->Config.a_current;
		}		
	}
}
