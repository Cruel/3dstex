#include "util.h"

PixelFormat formatFromString(const std::string &formatStr)
{
	if (formatStr == "rgba8")          return RGBA8;
	else if (formatStr == "rgb8")      return RGB8;
	else if (formatStr == "rgba5551")  return RGBA5551;
	else if (formatStr == "rgb565")    return RGB565;
	else if (formatStr == "rgba4")     return RGBA4;
	else if (formatStr == "la8")       return LA8;
	else if (formatStr == "hilo8")     return HILO8;
	else if (formatStr == "l8")        return L8;
	else if (formatStr == "a8")        return A8;
	else if (formatStr == "la4")       return LA4;
	else if (formatStr == "l4")        return L4;
	else if (formatStr == "a4")        return A4;
	else if (formatStr == "etc1")      return ETC1;
	else if (formatStr == "etc1a4")    return ETC1A4;
	else if (formatStr == "auto-etc1") return AutoETC1;
	
	return DefaultFormat;
}

std::string stringFromFormat(PixelFormat format)
{
	switch (format) {
		case RGBA8:    return "rgba8";
		case RGB8:     return "rgb8";
		case RGBA5551: return "rgba5551";
		case RGB565:   return "rgb565";
		case RGBA4:    return "rgba4";
		case LA8:      return "la8";
		case HILO8:    return "hilo8";
		case L8:       return "l8";
		case A8:       return "a8";
		case LA4:      return "la4";
		case L4:       return "l4";
		case A4:       return "a4";
		case ETC1:     return "etc1";
		case ETC1A4:   return "etc1a4";
		case AutoETC1: return "auto-etc1";
		default:       return "default";
	}
}

int bitsPerPixel(PixelFormat format)
{
	switch (format) {
		case RGBA8:
			return 32;
		case RGB8:
			return 24;
		case RGBA5551:
		case RGB565:
		case RGBA4:
		case LA8:
		case HILO8:
			return 16;
		case L8:
		case A8:
		case LA4:
		case ETC1A4:
			return 8;
		case L4:
		case A4:
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

// Grabbed from Citra Emulator (citra/src/video_core/utils.h)
static inline u32 morton_interleave(u32 x, u32 y)
{
	u32 i = (x & 7) | ((y & 7) << 8); // ---- -210
	i = (i ^ (i << 2)) & 0x1313;      // ---2 --10
	i = (i ^ (i << 1)) & 0x1515;      // ---2 -1-0
	i = (i | (	i >> 7)) & 0x3F;
	return i;
}

//Grabbed from Citra Emulator (citra/src/video_core/utils.h)
static inline u32 get_morton_offset(u32 x, u32 y, u32 bytes_per_pixel)
{
	u32 i = morton_interleave(x, y);
	u32 offset = (x & ~7) * 8;
	return (i + offset) * bytes_per_pixel;
}

void tileData(u8* dest, const u8* source, u32 x, u32 y, u32 src_w, u32 src_h, u32 dest_w, u32 dest_h)
{
	u32 i, j;
	for (j = 0; j < src_h; j++) {
		for (i = 0; i < src_w; i++) {

			int pos_y = (dest_h - 1 - j - y);

			u32 coarse_y = pos_y & ~7;
			u32 dst_offset = get_morton_offset(i+x, pos_y, 4) + coarse_y * dest_w * 4;

			u32 v = ((u32 *)source)[i + j*src_w];
			*(u32 *)(dest + dst_offset) = __builtin_bswap32(v);
		}
	}
}

void untileData(u8* dest, const u8* source, u32 x, u32 y, u32 src_w, u32 src_h, u32 dest_w, u32 dest_h)
{
	u32 i, j;
	for (j = 0; j < src_h; j++) {
		for (i = 0; i < src_w; i++) {

			int pos_y = (dest_h - 1 - j - y);

			u32 coarse_y = pos_y & ~7;
			u32 src_offset = get_morton_offset(i+x, pos_y, 4) + coarse_y * dest_w * 4;

			u32 v = *(u32 *)(source + src_offset);
			((u32 *)dest)[i + j*src_w] = __builtin_bswap32(v);
		}
	}
}
