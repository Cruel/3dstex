#pragma once

#include "types.h"
#include <memory>
#include <vector>
#include "rg_etc1.h"

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
	void encodeETC1Block(int blockX, int blockY, const u8 *decodedData, u8 *ptrOut);
	
private:
	PixelFormat m_formatRequested; //< User provided format, could be special format.
	PixelFormat m_formatUsed;      //< Format used for encoding.
	std::vector<u8> m_encodedData;
	
	Header m_header; //< Header to use in output, if requested
	bool m_hasAlpha;
	
	int m_qualityETC1;
	rg_etc1::etc1_pack_params m_etc1params;
};
