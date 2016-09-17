#include "Decoder.h"
#include "util.h"
#include "rg_etc1.h"
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>


Decoder::Decoder(const std::string &filePath, PixelFormat format)
	: m_format(format)
{
	if (format == DefaultFormat && loadImageFile(filePath))
		return;
	if (!loadTextureFile(filePath))
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
	// Open at end of file to read size first thing
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		// Get file size
		size_t size = file.tellg();
		
		file.seekg(0, file.beg);
		if (size < sizeof(Header))
			return false;
		size -= sizeof(Header);
		
		// Get header and validate the file size
		file.read(reinterpret_cast<char*>(&m_header), sizeof(Header));
		m_format = static_cast<PixelFormat>(m_header.format);
		m_width = m_header.widthOriginal;
		m_height = m_header.heightOriginal;
		if (m_header.width * m_header.height * bitsPerPixel(m_format) / 8 != size)
			return false;
		
		std::vector<u8> data;
		data.resize(size);
		file.read(reinterpret_cast<char*>(data.data()), size);
		
		decode(data);
		
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

void Decoder::decode(const std::vector<u8> &encodedData)
{
	std::vector<u8> tiledData;
	int pixelCount = m_header.width * m_header.height;
	tiledData.resize(pixelCount * 4);
	m_decodedData.resize(m_width * m_height * 4);
	
	const u8 *src = encodedData.data();
	const u16 *src16 = reinterpret_cast<const u16*>(encodedData.data());
	u8 *dest = tiledData.data();
	
	int i;
	if (m_format == RGBA8)
		tiledData = encodedData;
	else if (m_format == RGB8)
		for (i = 0; i < pixelCount; ++i)
		{
			*dest++ = 255;
			*dest++ = *src++;
			*dest++ = *src++;
			*dest++ = *src++;
		}
	else if (m_format == RGBA5551)
		for (i = 0; i < pixelCount; ++i)
		{
			u16 pixel = *src16++;
			*dest++ = Convert1To8(pixel & 0x1);
			*dest++ = Convert5To8((pixel >> 1) & 0x1F);
			*dest++ = Convert5To8((pixel >> 6) & 0x1F);
			*dest++ = Convert5To8((pixel >> 11) & 0x1F);
		}
	else if (m_format == RGB565)
		for (i = 0; i < pixelCount; ++i)
		{
			u16 pixel = *src16++;
			*dest++ = 255;
			*dest++ = Convert5To8(pixel & 0x1F);
			*dest++ = Convert6To8((pixel >> 5) & 0x3F);
			*dest++ = Convert5To8((pixel >> 11) & 0x1F);
		}
	else if (m_format == RGBA4)
		for (i = 0; i < pixelCount; ++i)
		{
			u16 pixel = *src16++;
			*dest++ = Convert4To8(pixel & 0xF);
			*dest++ = Convert4To8((pixel >> 4) & 0xF);
			*dest++ = Convert4To8((pixel >> 8) & 0xF);
			*dest++ = Convert4To8((pixel >> 12) & 0xF);
		}
	else if (m_format == LA8)
		for (i = 0; i < pixelCount; ++i)
		{
			*dest++ = *src++;
			u8 L = *src++;
			*dest++ = L; *dest++ = L; *dest++ = L;
		}
	else if (m_format == HILO8)
		for (i = 0; i < pixelCount; ++i)
		{
			*dest++ = 255;
			*dest++ = 0;
			*dest++ = *src++;
			*dest++ = *src++;
		}
	else if (m_format == L8)
		for (i = 0; i < pixelCount; ++i)
		{
			u8 L = *src++;
			*dest++ = 255;
			*dest++ = L; *dest++ = L; *dest++ = L;
		}
	else if (m_format == A8)
		for (i = 0; i < pixelCount; ++i)
		{
			*dest++ = *src++;
			*dest++ = 0; *dest++ = 0; *dest++ = 0;
		}
	else if (m_format == LA4)
		for (i = 0; i < pixelCount; ++i)
		{
			u8 pixel = *src++;
			*dest++ = Convert4To8(pixel & 0xF);
			u8 L = Convert4To8((pixel >> 4) & 0xF);
			*dest++ = L; *dest++ = L; *dest++ = L;
		}
	else if (m_format == L4)
		for (i = 0; i < pixelCount / 2; ++i)
		{
			u8 pixel = *src++;
			u8 L1 = Convert4To8(pixel & 0xF);
			u8 L2 = Convert4To8((pixel >> 4) & 0xF);
			*dest++ = 255;
			*dest++ = L1; *dest++ = L1; *dest++ = L1;
			*dest++ = 255;
			*dest++ = L2; *dest++ = L2; *dest++ = L2;
		}
	else if (m_format == A4)
		for (i = 0; i < pixelCount / 2; ++i)
		{
			u8 pixel = *src++;
			u8 A1 = Convert4To8(pixel & 0xF);
			u8 A2 = Convert4To8((pixel >> 4) & 0xF);
			*dest++ = A1;
			*dest++ = 0; *dest++ = 0; *dest++ = 0;
			*dest++ = A2;
			*dest++ = 0; *dest++ = 0; *dest++ = 0;
		}
		
	// Finally untile data
	untileData(m_decodedData.data(), tiledData.data(), 0, 0,
	           m_header.width, m_header.height, m_width, m_height);
}
