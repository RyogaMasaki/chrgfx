#include "nintendo_gb_gfx.hpp"

using namespace png;

namespace chrgfx
{
// ----------------- CHR
const chr_traits nintendo_gb_cx::traits = std_2bpp_tile;

const u8 CHR_PXL_COUNT = 64;

const chr_traits* nintendo_gb_cx::get_traits()
{
	return &nintendo_gb_cx::traits;
}

const chr* nintendo_gb_cx::get_chr(u8* data) { return get_chr_gb(data); }

const chr* nintendo_gb_cx::get_chr_gb(u8* data)
{
	auto _out = new chr[CHR_PXL_COUNT];
	u8 this_pxl, pxl_idx{0};

	for(u8 row = 0; row < 16; row += 2)
	{
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl |= 1;
			if((data[row + 1] >> shift) & 1) this_pxl |= 2;
			_out[pxl_idx++] = this_pxl;
		}
	}

	return _out;
}

// ----------------- PALETTES
// 4 colors, 2 bits per color (1 byte per palette)
// 3 palettes (3 bytes, stored at 0xFF47 in memory)
const pal_traits nintendo_gb_px::traits = {12, 1};

const pal_traits* nintendo_gb_px::get_traits()
{
	return &nintendo_gb_px::traits;
}

// colors based on the yellow-green screen of the original GameBoy
// (based on MAME output)
const color nintendo_gb_px::gameboy_colors_original[] = {
		color(0xff, 0xfb, 0x87),	 // 00 - lightest
		color(0xb1, 0xae, 0x4e),	 // 01
		color(0x84, 0x80, 0x4e),	 // 02
		color(0x4e, 0x4e, 0x4e)};	// 03 - darkest

// colors based on the gray screen of the GameBoy Pocket
// (based on MAME output)
const color nintendo_gb_px::gameboy_colors_pocket[] = {
		color(0xc4, 0xcf, 0xa1),	 // 00 - lightest
		color(0x8b, 0x95, 0x6d),	 // 01
		color(0x6b, 0x73, 0x53),	 // 02
		color(0x41, 0x41, 0x41)};	// 03 - darkest

const color* nintendo_gb_px::get_rgb(u8* data)
{
	if(*data > 3) throw std::out_of_range("Invalid GameBoy palette entry");
	return &gameboy_colors_original[*data];
}

const palette* nintendo_gb_px::get_gb_pal(u8* data, const color* gb_palette)
{
	auto _out = new palette();
	_out->reserve(12);

	// 3 palettes (BGRD, OBJ0, OBJ1)
	// 8 bits, every 2 bits is a palette entry
	// 3  2  1  0   pal entry
	// 11 11 11 11  bit
	for(u8 pal_iter = 0; pal_iter < 3; pal_iter++)
	{
		for(u8 shift = 0; shift < 8; shift += 2)
		{
			u8 this_entry = (data[pal_iter] >> shift) & 3;
			_out->push_back(gb_palette[this_entry]);
		}
	}

	return _out;
}

const palette* nintendo_gb_px::get_pal(u8* data)
{
	return nintendo_gb_px::get_gb_pal(data,
																		nintendo_gb_px::gameboy_colors_original);
}

const palette* nintendo_gbpocket_px::get_pal(u8* data)
{
	return nintendo_gb_px::get_gb_pal(data,
																		nintendo_gb_px::gameboy_colors_pocket);
}

const color* nintendo_gbpocket_px::get_rgb(u8* data)
{
	if(*data > 3) throw std::out_of_range("Invalid GameBoy palette entry");
	return &gameboy_colors_pocket[*data];
}

// GameBoy Color palettes

const pal_traits nintendo_gbcolor_px::traits = {32, 2};

const pal_traits* nintendo_gbcolor_px::get_traits()
{
	return &nintendo_gbcolor_px::traits;
}

const color* nintendo_gbcolor_px::get_rgb(u8* data)
{
	// 16|               |0
	//   xBBBBBGG GGGRRRRR
	// (little endian in rom, big endian in memory; process as big endian)
	u8 r = data[1] & 0x1F;
	u8 g = ((data[1] & 0xE0) >> 5) | ((data[0] & 3) << 3);
	u8 b = (data[0] & 0x7C) >> 2;

	r = (r << 3);
	g = (g << 3);
	b = (b << 3);

	return new color(r, g, b);
}

const palette* nintendo_gbcolor_px::get_pal(u8* data)
{
	return chrgfx::get_pal(this, data, 32);
}

}	// namespace chrgfx