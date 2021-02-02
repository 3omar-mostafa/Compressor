# Compressor
A Fast C++ Lossless Compress/Decompress CLI Program with High Compression Ratio

# Algorithms Implemented
- Huffman
- LZW *(Lempel – Ziv – Welch)*
- BWT *(Burrows - Wheeler Transform)* and MTF *(Move To Front)*

The Pipeline which produces best compression ratio is **`BWT -> MTF -> LZW`**

# Compatibility
Tested on Linux (Ubuntu) with GNU GCC and Windows with Microsoft Visual Studio and Mingw-w64

Requires CMake and C++11 or above

# How to Build
- Clone this repo with its submodules **`git clone --recurse-submodules https://github.com/3omar-mostafa/Compressor.git`**
- Use CMake to Build
```bash
mkdir cmake-build
cd ./cmake-build
cmake ..
```

You Can Create Visual Studio Project on Windows
```bash
mkdir cmake-build
cd ./cmake-build
cmake .. -G "Visual Studio 15 2017"
```

# How To Run
```
./Compressor OPTION input_file output_file
OPTION:
      -c  --compress     Compress the file
      -d  --decompress   Decompress the file
```

# Tests and Results
Tested on [enwik8](http://mattmahoney.net/dc/enwik8.zip) (Size of 100MB)

Tested on Github Action Servers with 2-core 64-Bit CPU, 7 GB of RAM memory and SSD. [Learn more about environment](https://docs.github.com/en/actions/reference/specifications-for-github-hosted-runners#supported-runners-and-hardware-resources)

Executables are built in release mode with max compiler optimization

### Compression Ratio on enwik8: 3.858, Lossless Integrity checked with SHA512

| Platform                | Compress Time (Avg) | Decompress Time (Avg) |
|:-----------------------:|:-------------------:|:---------------------:|
| Linux (Ubuntu) - GNU GCC|        1m 25s       |           20s         |
| Windows - Visual Studio |        4m 30s       |           46s         |
| Windows - Mingw-w64     |        4m 3s        |           36s         |

# TODO
 - [ ] More Memory Optimization

# Credits
Used this paper **`Linear Work Suffix Array Construction (2006)`** By **`Juha Kärkkäinen, Peter Sanders, Stefan Burkhardt`** 
to implement a faster Suffix Array **`O(n)`** instead of **`O(nlogn)`**
