#pragma once

#include "types.h"

PixelFormat formatFromString(const std::string &formatStr);
std::string stringFromFormat(PixelFormat format);

int bitsPerPixel(PixelFormat format);
int nextPow2(int size);
