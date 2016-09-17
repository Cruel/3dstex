#pragma once

#include "types.h"
#include <string>
#include <vector>

class Decoder
{
public:
	Decoder(const std::string &filePath, PixelFormat format);
	
	const std::vector<u8> &getDecodedData() const { return m_decodedData; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	bool hasAlpha() const { return m_hasAlpha; }
	PixelFormat getPixelFormat() { return m_format; }
	
private:
	bool loadImageFile(const std::string &filePath);
	bool loadTextureFile(const std::string &filePath);
	bool isOpaque();
	void decode(const std::vector<u8> &encodedData);
	
private:
	PixelFormat m_format;
	std::vector<u8> m_decodedData;
	Header m_header;
	bool m_hasAlpha;
	int  m_width;
	int  m_height;
};
