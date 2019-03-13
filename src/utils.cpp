#include "utils.h"
#include <png/png.h>
#include <fstream>
#include <memory>

static void userReadData(png_structp pngRead, png_bytep data, png_size_t length) {
	png_voidp f = png_get_io_ptr(pngRead);
	((std::ifstream*)f)->read((char*)data, length);
}

bool LoadPng(const char* filename, int& width, int& height, int& chan, 
	std::shared_ptr<std::vector<unsigned char>> & img) {
	// We need to declare these up here so they can be cleaned up
  // regardless of error condition.
	bool pngReadInited = false;
	bool pngInfoInited = false;
	png_structp pngRead;
	png_infop pngInfo;

	std::ifstream pngFile;
	pngFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		pngFile.open(filename, std::ios::binary);

		// Check the PNG header (8 bytes).
		png_byte header[8];
		pngFile.read((char*)header, 8);
		if (png_sig_cmp(header, 0, 8)) {
			throw PNGError("Bad PNG header");
		}

		// Read the PNG data using libpng.
		pngRead = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
		if (pngRead) {
			pngReadInited = true;
		}
		else {
			throw PNGError("Could not initialize PNG read");
		}

		pngInfo = png_create_info_struct(pngRead);
		if (pngInfo) {
			pngInfoInited = true;
		}
		else {
			throw PNGError("Could not initialize PNG info");
		}

		// This error-handling method is prescribed in the libpng manual.
		if (setjmp(png_jmpbuf(pngRead))) {
			// An error occurred, so clean up now.
			throw PNGError("Error occurred while reading PNG");
		}

		// Have libpng read from our C++ stream.
		png_set_read_fn(pngRead, (png_voidp)&pngFile, userReadData);

		// Tell libpng we've already read the 8-byte header.
		png_set_sig_bytes(pngRead, 8);

		// Read the entire PNG header.
		png_read_info(pngRead, pngInfo);

		png_uint_32 pngWidth = png_get_image_width(pngRead, pngInfo);
		png_uint_32 pngHeight = png_get_image_height(pngRead, pngInfo);
		width = pngWidth;
		height = pngHeight;

		png_uint_32 bitsPerChannel = png_get_bit_depth(pngRead, pngInfo);
		png_uint_32 channels = png_get_channels(pngRead, pngInfo);
		png_uint_32 colorType = png_get_color_type(pngRead, pngInfo);

		switch (colorType) {
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(pngRead);
			channels = 3;
			break;
		case PNG_COLOR_TYPE_RGB:
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			break;
		default:
			throw PNGError("Unsupported color type");
		}

		// Convert any transparency to a full alpha channel.
		if (png_get_valid(pngRead, pngInfo, PNG_INFO_tRNS)) {
			png_set_tRNS_to_alpha(pngRead);
			channels += 1;
		}

		// Convert 16-bit precision to 8-bit precision.
		if (bitsPerChannel == 16) png_set_strip_16(pngRead);

		std::shared_ptr<std::vector<png_bytep>> rows = std::make_shared<std::vector<png_bytep>>(pngHeight);
		// Note: divide by 8 bits/1 byte.
		std::shared_ptr<std::vector<png_byte>> pngBytes = std::make_shared<std::vector<png_byte>>(pngWidth *
			pngHeight * bitsPerChannel * channels / 8);
		// Length in bytes of one row.
		const unsigned int stride = pngWidth * bitsPerChannel * channels / 8;

		for (size_t row = 0; row < pngHeight; row++) {
			// We're setting the pointers "upside-down".
			png_uint_32 offset = (pngHeight - row - 1) * stride;
			(*rows)[row] = (png_bytep)(pngBytes->data()) + offset;
		}

		// Actually read the image!
		png_read_image(pngRead, rows->data());

		if (pngReadInited && pngInfoInited) {
			png_destroy_read_struct(&pngRead, &pngInfo, (png_infopp)NULL);
		}

		//_rows = rows;
		img = pngBytes;
		chan = channels;
	}
	catch (...) {
		// Clean up data.
		if (pngReadInited && pngInfoInited) {
			png_destroy_read_struct(&pngRead, &pngInfo, (png_infopp)NULL);
		}
		throw;
	}
}