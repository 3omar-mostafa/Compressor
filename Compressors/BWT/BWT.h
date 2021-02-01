#ifndef BWT_H
#define BWT_H

#include "../../Utils/BinaryIO.h"
#include "SuffixArray.h"
#include <algorithm>
#include <cstdint>
#include <deque>
#include <bit_string.h>

/**
 * Burrows - Wheeler Transform
 */
class BWT {

    static uint32_t originalIndex;

public:

    static void encode(const std::string& filename, const std::string& outputFileName) {

        std::string toBeEncoded = BinaryIO::readString(filename);
        toBeEncoded += '\0'; // Last char must be the smallest of all

        std::vector<uint32_t> suffixArray = SuffixArray::buildSuffixArray(toBeEncoded);

        std::string bwt = generateBWT(toBeEncoded, suffixArray);

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, bit_string::from_uint_32(originalIndex));
        BinaryIO::write(outputFileName, bwt);

    }

    static void decode(const std::string& filename, const std::string& outputFileName) {

        // Original Index is the first 4 bytes of the file
        originalIndex = BinaryIO::readBitString(filename, 0, sizeof(originalIndex)).to_uint_32();
        std::string bwt = BinaryIO::readString(filename, sizeof(originalIndex));

        std::string inverseBWT = BWT::invertBWT(bwt, originalIndex);
        inverseBWT.pop_back(); // remove '\0' which was added at encoding

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, inverseBWT);

    }


private:

    static std::vector<uint32_t> computeLeftShift(const std::string& bwt) {

        // Each element is a list of all indices the this element appeared at in bwt
        // example: symbolsIndicesList[65] contains all indices that letter 'A' appears at
        std::deque<uint32_t> symbolsIndicesList[256]; // 256 is the length all symbols as we us byte it is 256 maximum
        for (int i = 0; i < bwt.length(); i++) {
            symbolsIndicesList[(uint8_t) bwt[i]].push_back(i);
        }

        std::string sortedBWT = bwt;
        std::sort(sortedBWT.begin(), sortedBWT.end(), unsignedCharsCompare);

        std::vector<uint32_t> leftShift(bwt.length());
        // Computes Left Shift
        for (int i = 0; i < bwt.length(); i++) {
            std::deque<uint32_t>& symbolIndices = symbolsIndicesList[(uint8_t) sortedBWT[i]];
            leftShift[i] = symbolIndices.front();
            symbolIndices.pop_front();
        }

        return leftShift;
    }

    // Generate Burrows - Wheeler Transform of given text
    static std::string generateBWT(const std::string& input, std::vector<uint32_t>& suffixArray) {
        // Iterates over the suffix array to find
        // the last char of each cyclic rotation
        std::string bwtLastColumn;

        int n = input.length();
        for (int i = 0; i < suffixArray.size(); ++i) {
            // Computes the last char which is given by
            // input[(suffix + n - 1) % n]
            int j = suffixArray[i] - 1;
            if (j < 0) {
                j += n;
            }

            if (suffixArray[i] == 0)
                originalIndex = i;

            bwtLastColumn += input[j];
        }

        return bwtLastColumn;
    }

    static bool unsignedCharsCompare(const char c1, const char c2) {
        return (unsigned char) c1 < (unsigned char) c2;
    }

    static std::string invertBWT(const std::string& BWT, long long index) {

        std::vector<uint32_t> leftShift = computeLeftShift(BWT);

        std::string inverseBWT;
        inverseBWT.reserve(BWT.length());

        // Decodes the bwt
        for (int i = 0; i < BWT.length(); i++) {
            index = leftShift[index];
            inverseBWT += BWT[index];
        }

        return inverseBWT;
    }

};

uint32_t BWT::originalIndex = 0;

#endif //BWT_H
