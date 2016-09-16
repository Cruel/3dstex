#pragma once

#include "types.h"
#include <memory>
#include <vector>

class Decoder;

class Encoder
{
public:
	Encoder(PixelFormat format, int qualityETC1);
	
	void saveToFile(const std::string &filePath, bool useHeader);
	void processDecodedData(const Decoder &decoder);
	void processDecodedData(const std::vector<u8> &decodedData, int width, int height, bool hasAlpha);
	PixelFormat getEncodedFormat() { return m_formatUsed; }
	const std::vector<u8> &getEncodedData() { return m_encodedData; }
	
private:
	void encode(const std::vector<u8> &decodedData);
	void encodeETC1(const std::vector<u8> &decodedData);
	
private:
	PixelFormat m_formatRequested; //< User provided format, could be special format.
	PixelFormat m_formatUsed;      //< User provided format, could be special format.
	std::vector<u8> m_encodedData;
	
	int m_qualityETC1;
	Header m_header; //< Header to use in output, if requested
	bool m_hasAlpha;
};
