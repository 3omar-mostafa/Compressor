#ifndef LZW_UTILS_H
#define LZW_UTILS_H

#include <cstdint>

#ifdef __GNUC__ // GNU GCC Compiler
/**
 * Returns the number of bits to store range of [n] numbers from 0 to n-1 <br>
 * Fast (Constant Time O(1)) due to use of builtin intrinsics
 */
inline uint32_t numberOfBitsToStoreRangeOf(uint32_t n) {
    if(n == 0 || n == 1)
        return n;

    // __builtin_clz -> count the number of leading zero bits in the int number
    // After subtracting the size of the variable (32 bits), we have the first 1 bit from the left (MSB direction)
    return (32 - __builtin_clz(n - 1));
}

#elif defined(_MSC_VER) // Microsoft Visual Studio Compiler
#include <intrin.h>

/**
 * Returns the number of bits to store range of [n] numbers from 0 to n-1 <br>
 * Fast (Constant Time O(1)) due to use of builtin intrinsics
 */
inline uint32_t numberOfBitsToStoreRangeOf(uint32_t n) {
    if(n == 0 || n == 1)
        return n;

    unsigned long numberOfBits = 0;
    _BitScanReverse(&numberOfBits, n - 1); // Returns the position of the first 1 bit from the left (MSB direction)
    return numberOfBits+1; // Adding 1 because position is zero based
}
#else
#include <cmath>

/**
 * Returns the number of bits to store range of [n] numbers from 0 to n-1 <br>
 * Slower than others due to using of normal maths
 */
inline uint32_t numberOfBitsToStoreRangeOf(uint32_t n) {
    if(n == 0 || n == 1)
        return n;

    return uint32_t(log2(n-1)) + 1;
}
#endif

#endif //LZW_UTILS_H
