#pragma once

#include "types.h"

PixelFormat formatFromString(const std::string &formatStr);
std::string stringFromFormat(PixelFormat format);

int bitsPerPixel(PixelFormat format);
int nextPow2(int size);

void tileData(u8* dest, const u8* source, u32 x, u32 y, u32 src_w, u32 src_h, u32 dest_w, u32 dest_h);
void untileData(u8* dest, const u8* source, u32 x, u32 y, u32 src_w, u32 src_h, u32 dest_w, u32 dest_h);
