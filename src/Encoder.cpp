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
