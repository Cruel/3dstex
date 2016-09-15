#pragma once

#include <stdint.h>
#include <string>

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

enum PixelFormat
{
	// Standard types.
	// Must never modify these to maintain header compatibility.
	RGBA8,
	RGB8,
	RGBA5551,
	RGB565,
	RGBA4,
	LA8,
	HILO8,
	L8,
	A8,
	LA4,
	A4,
	ETC1,
	ETC1A4,

	// Special types.
	DefaultFormat,
	AutoETC1,
};

typedef struct
{
	u8 format;
	u16 width;
	u16 height;
	u16 widthOriginal;
	u16 heightOriginal;
} __attribute__((packed)) Header;

typedef struct
{
	PixelFormat formatInput;
	PixelFormat formatOutput;
	bool useHeader;
	int etc1quality;
	int threadCount;
	std::string outputDir;
} Options;
