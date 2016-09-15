#include "Decoder.h"
#include "rg_etc1.h"
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>


Decoder::Decoder(const std::string &filePath, PixelFormat format)
	: m_format(format)
{
	if ((format == DefaultFormat && !loadImageFile(filePath)) || !loadTextureFile(filePath))
		throw 0;
}

bool Decoder::loadImageFile(const std::string &filePath)
{
	// Fetch original bits per pixel, but request data with 4 bpp
	int bitsPerPixel;
	u8 *data = stbi_load(filePath.c_str(), &m_width, &m_height, &bitsPerPixel, 4);
	if (data)
	{
		m_decodedData.assign(data, data + m_width * m_height * 4);
		m_hasAlpha = (bitsPerPixel == 4 && !isOpaque());
		stbi_image_free(data);
		return true;
	}
	return false;
}

bool Decoder::loadTextureFile(const std::string &filePath)
{
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		// Get filesize
		size_t size = file.tellg();
		file.seekg(0, file.end);
		if (size < sizeof(Header))
			return false;
		
		// Get header
		Header header;
		file.read(reinterpret_cast<char*>(&header), sizeof(Header));
		
		return true;
	}
	return false;
}

bool Decoder::isOpaque()
{
	for (u32 i = 3; i < m_decodedData.size(); i += 4)
		if (m_decodedData[i] < 255)
			return false;
	return true;
}
