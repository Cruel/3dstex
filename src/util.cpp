#include "util.h"

PixelFormat formatFromString(const std::string &formatStr)
{
	if (formatStr == "rgba8") return RGBA8;
	else if (formatStr == "rgb8") return RGB8;
	else if (formatStr == "etc1") return ETC1;
	else if (formatStr == "etc1a4") return ETC1A4;
	else if (formatStr == "auto-etc1") return AutoETC1;
	else return DefaultFormat;
}

std::string stringFromFormat(PixelFormat format)
{
	switch (format) {
		case RGBA8:    return "rgba8";
		case RGB8:     return "rgb8";
		case ETC1:     return "etc1";
		case ETC1A4:   return "etc1a4";
		case AutoETC1: return "auto-etc1";
		default: return "default";
	}
}

int bitsPerPixel(PixelFormat format)
{
	switch (format) {
		case RGBA8:
			return 32;
		case RGB8:
			return 24;
		case ETC1A4:
			return 8;
		case ETC1:
			return 4;
		default:
			return 0;
	}
}

int nextPow2(int size)
{
	int sizePOT = 1;
	while (sizePOT < size)
		sizePOT *= 2;
	return sizePOT;
}
