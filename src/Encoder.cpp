#include "Encoder.h"
#include "Decoder.h"
#include "util.h"
#include "rg_etc1.h"
#include <fstream>
#include <cstring>


Encoder::Encoder(PixelFormat format, int qualityETC1)
	: m_formatRequested(format)
	, m_formatUsed(format)
	, m_qualityETC1(qualityETC1)
{
	//
}

void Encoder::saveToFile(const std::string &filePath, bool useHeader)
{
	std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
	if (file.is_open())
	{
		if (useHeader)
			file.write(reinterpret_cast<char*>(&m_header), sizeof(Header));
		file.write(reinterpret_cast<char*>(&m_encodedData[0]), m_encodedData.size());
	}
}

void Encoder::processDecodedData(const std::vector<u8> &data, int width, int height, bool hasAlpha)
{
	m_header.widthOriginal = width;
	m_header.heightOriginal = height;
	m_header.width = nextPow2(width);
	m_header.height = nextPow2(height);
	m_hasAlpha = hasAlpha;
	
	// Check for special formats and set appropriate one
	if (m_formatRequested == AutoETC1)
		m_formatUsed = m_hasAlpha ? ETC1A4 : ETC1;
	else if (m_formatRequested == AutoL8)
		m_formatUsed = m_hasAlpha ? LA8 : L8;
	else if (m_formatRequested == AutoL4)
		m_formatUsed = m_hasAlpha ? LA4 : L4;
	
	m_header.format = m_formatUsed;
	m_encodedData.resize(width * height * bitsPerPixel(m_formatUsed) / 8);
// 	std::fill(m_encodedData.begin(), m_encodedData.end(), 0);
	
	if (m_formatUsed == ETC1 || m_formatUsed == ETC1A4)
		encodeETC1(data);
	else
		encode(data);
}

void Encoder::processDecodedData(const Decoder& decoder)
{
	processDecodedData(
		decoder.getDecodedData(),
		decoder.getWidth(),
		decoder.getHeight(),
		decoder.hasAlpha()
  	);
}

void Encoder::encode(const std::vector<u8> &decodedData)
{
	// Get the tiled data before converting color format.
	std::vector<u8> data;
	data.resize(decodedData.size());
	tileData(data.data(), decodedData.data(), 0, 0,
	         m_header.width, m_header.height, m_header.width, m_header.height);
	
	u8 *src = data.data();
	u8 *dest8 = m_encodedData.data();
	u16 *dest16 = reinterpret_cast<u16*>(m_encodedData.data());
	
	if (m_formatUsed == RGBA8)
	{
		m_encodedData = data;
		return;
	}

	// Loop through all pixels and convert them
	int i;
	if (m_formatUsed == RGB8)
		for (i = 0; i < m_header.width * m_header.height; ++i)
		{
			src++;
			*dest8++ = *src++;
			*dest8++ = *src++;
			*dest8++ = *src++;
		}
	else if (m_formatUsed == RGBA5551)
		for (i = 0; i < m_header.width * m_header.height; ++i)
		{
			u16 A = Convert8To1(*src++);
			u16 B = Convert8To5(*src++) << 1;
			u16 G = Convert8To5(*src++) << 6;
			u16 R = Convert8To5(*src++) << 11;
			*dest16++ = R | G | B | A;
		}
	else if (m_formatUsed == RGB565)
		for (i = 0; i < m_header.width * m_header.height; ++i)
		{
			src++;
			u16 B = Convert8To5(*src++);
			u16 G = Convert8To6(*src++) << 5;
			u16 R = Convert8To5(*src++) << 11;
			*dest16++ = R | G | B;
		}
	else if (m_formatUsed == RGBA4)
		for (i = 0; i < m_header.width * m_header.height; ++i)
		{
			u16 A = Convert8To4(*src++);
			u16 B = Convert8To4(*src++) << 4;
			u16 G = Convert8To4(*src++) << 8;
			u16 R = Convert8To4(*src++) << 12;
			*dest16++ = R | G | B | A;
		}
	else if (m_formatUsed == LA8)
		for (i = 0; i < m_header.width * m_header.height; ++i)
		{
			*dest8++ = *src++;
			*dest8++ = ProcessBGR8ToL(src);
		}
	else if (m_formatUsed == HILO8)
		for (i = 0; i < m_header.width * m_header.height; ++i)
		{
			src += 2;
			*dest8++ = *src++;
			*dest8++ = *src++;
		}
	else if (m_formatUsed == L8)
		for (i = 0; i < m_header.width * m_header.height; ++i)
		{
			src++;
			*dest8++ = ProcessBGR8ToL(src);
		}
	else if (m_formatUsed == A8)
		for (i = 0; i < m_header.width * m_header.height; ++i)
		{
			*dest8++ = *src;
			src += 4;
		}
	else if (m_formatUsed == LA4)
		for (i = 0; i < m_header.width * m_header.height; ++i)
		{
			u8 A = *src++;
			u8 L = ProcessBGR8ToL(src);
			*dest8++ = (A >> 4) | ((L >> 4) << 4);
		}
	else if (m_formatUsed == L4)
		for (i = 0; i < m_header.width * m_header.height / 2; ++i)
		{
			// Pre-increment to skip alpha channel
			u8 L1 = ProcessBGR8ToL(++src);
			u8 L2 = ProcessBGR8ToL(++src);
			*dest8++ = (L1 >> 4) | ((L2 >> 4) << 4);
		}
	else if (m_formatUsed == A4)
		for (i = 0; i < m_header.width * m_header.height / 2; ++i)
		{
			u8 lo4 = *src >> 4;
			src += 4;
			u8 hi4 = (*src >> 4) << 4;
			src += 4;
			*dest8++ = hi4 | lo4;
		}
}

void Encoder::encodeETC1(const std::vector<u8> &decodedData)
{
	u8 *ptrOut = m_encodedData.data();
	
	rg_etc1::etc1_pack_params params;
	params.m_quality = static_cast<rg_etc1::etc1_quality>(m_qualityETC1 - 1);
	
	rg_etc1::pack_etc1_block_init();
	
	for (int y = 0; y < m_header.height; y += 8)
	{
		for (int x = 0; x < m_header.width; x += 8)
		{
			for (int i = 0; i < 8; i += 4)
			{
				for (int j = 0; j < 8; j += 4)
				{
					u64 block = 0;
					u32 pixels[4 * 4];

					if (m_formatUsed == ETC1A4)
					{
						int iiii = 0;
						for (int xx = 0; xx < 4; xx++)
						{
							for (int yy = 0; yy < 4; yy++)
							{
								u32 color;
								if (x + j + xx >= m_header.widthOriginal)
									color = 0x00FFFFFF;
								else if (y + i + yy >= m_header.heightOriginal)
									color = 0x00FFFFFF;
								else
									color = decodedData[((m_header.height - 1 - y - i - yy) * m_header.widthOriginal) + x + j + xx];
								u32 a = color >> 24;
								a >>= 4;
								block |= (u64)a << (iiii * 4);
								iiii++;
							}
						}
						std::memcpy(ptrOut, &block, 8);
						ptrOut += 8;
					}

					for (int yy = 0; yy < 4; yy++)
					{
						for (int xx = 0; xx < 4; xx++)
						{
							// If pixel is outside of real image bounds, make it transparent
							if (x + j + xx >= m_header.widthOriginal)
								pixels[yy * 4 + xx] = 0;
							else if (y + i + yy >= m_header.heightOriginal)
								pixels[yy * 4 + xx] = 0;
							else
							{
								const u8 *d = &decodedData[(((m_header.height - 1 - y - i - yy) * m_header.widthOriginal) + x + j + xx) * 4];
								u8 *p = (u8*)&pixels[yy * 4 + xx];
								p[3] = 255;
								p[2] = d[2];
								p[1] = d[1];
								p[0] = d[0];
							}
						}
					}

					// TODO: Add threading to block packing
					rg_etc1::pack_etc1_block(&block, pixels, params);
					block = __builtin_bswap64(block);
					std::memcpy(ptrOut, &block, 8);
					ptrOut += 8;
				}
			}
		}
	}
}
