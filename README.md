# 3dstex
Texture conversion utility for Nintendo 3DS homebrew.

------------

```
Usage:
  ./3dstex -o <format> [options] files ...

Options:
  -r           Raw output without header. Header is added by default.
  -p           Print info of input files instead of encoding them.
  -h           Print this help information.
  -d <dir>     Directory to output files. Defaults to directory of inputs.
  -c <level>   Quality level for ETC1 compression:
                 1  - Low quality    (fast)
                 2  - Medium quality (default)
                 3  - High quality   (slow)
  -i <format>  Input format type. Only needed when input files have no header.
               Possible types: rgba8, rgb8, rgba5551, rgb565, rgba4, la8, hilo8,
               l8, a8, la4, a4, etc1, etc1a4.
  -o <format>  Output format type. Can be any of the above input types with some
               additional special types:
                 auto-etc1  - Uses ETC1 when input has no alpha data,
                              otherwise uses ETC1A4.
```