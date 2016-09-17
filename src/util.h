#pragma once

#include "types.h"

PixelFormat formatFromString(const std::string &formatStr);
std::string stringFromFormat(PixelFormat format);

u32 bitsPerPixel(PixelFormat format);
u32 nextPow2(u32 size);

void tileData(u8* dest, const u8* source, u32 x, u32 y, u32 src_w, u32 src_h, u32 dest_w, u32 dest_h);
void untileData(u8* dest, const u8* source, u32 x, u32 y, u32 src_w, u32 src_h, u32 dest_w, u32 dest_h);

inline u8 ProcessBGR8ToL(u8 *&bytes) {
	u8 L = *bytes++ * 0.0722f;
	L += *bytes++ * 0.7152f;
	L += *bytes++ * 0.2126f;
    return L;
}

// Convert helpers from Citra Emulator (citra/src/common/color.h)

/// Convert a 1-bit color component to 8 bit
constexpr u8 Convert1To8(u8 value) {
    return value * 255;
}

/// Convert a 4-bit color component to 8 bit
constexpr u8 Convert4To8(u8 value) {
    return (value << 4) | value;
}

/// Convert a 5-bit color component to 8 bit
constexpr u8 Convert5To8(u8 value) {
    return (value << 3) | (value >> 2);
}

/// Convert a 6-bit color component to 8 bit
constexpr u8 Convert6To8(u8 value) {
    return (value << 2) | (value >> 4);
}

/// Convert a 8-bit color component to 1 bit
constexpr u8 Convert8To1(u8 value) {
    return value >> 7;
}

/// Convert a 8-bit color component to 4 bit
constexpr u8 Convert8To4(u8 value) {
    return value >> 4;
}

/// Convert a 8-bit color component to 5 bit
constexpr u8 Convert8To5(u8 value) {
    return value >> 3;
}

/// Convert a 8-bit color component to 6 bit
constexpr u8 Convert8To6(u8 value) {
    return value >> 2;
}
