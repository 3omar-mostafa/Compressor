#ifndef LZW_H
#define LZW_H

#include <string>
#include <unordered_map>
#include <cstdint>
#include "../../Utils/BinaryIO.h"
#include "../../Utils/Converter.h"
#include "Utils.h"
#include <bit_string.h>

/**
 * Lempel–Ziv–Welch Compression Algorithm
 */
class LZW {

    static const uint32_t BYTE = 8;

    static std::unordered_map<std::string, uint32_t> initializeEncodingDictionary() {
        std::unordered_map<std::string, uint32_t> encodingDictionary(256);
        for (int i = 0; i < 256; ++i) {
            std::string s(1, i);
            encodingDictionary[s] = i;
        }
        return encodingDictionary;
    }

    static std::unordered_map<uint32_t, std::string> initializeDecodingDictionary() {
        std::unordered_map<uint32_t, std::string> decodingDictionary(256);
        for (int i = 0; i < 256; ++i) {
            std::string s(1, i);
            decodingDictionary[i] = s;
        }
        return decodingDictionary;
    }

public:


    static void encode(const std::string& filename, const std::string& outputFileName) {

        auto dictionary = initializeEncodingDictionary();

        std::string toBeCompressed = BinaryIO::readString(filename);
        bit_string encodedData;
        encodedData.reserve(toBeCompressed.size() / (4 * BYTE));

        uint32_t currentWordLength;
        std::string currentMatch;
        for (uint8_t byte : toBeCompressed) {
            currentMatch += byte;

            // if not found in dictionary
            if (dictionary.find(currentMatch) == dictionary.end()) {
                dictionary[currentMatch] = dictionary.size();
                currentWordLength = numberOfBitsToStoreRangeOf(dictionary.size());
                currentMatch.pop_back();
                encodedData.append_uint_32(dictionary[currentMatch], currentWordLength);
                currentMatch = byte;
            }

        }

        // save last matched word
        currentWordLength = numberOfBitsToStoreRangeOf(dictionary.size());
        encodedData.append_uint_32(dictionary[currentMatch], currentWordLength);

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, encodedData);
    }


    static void decode(const std::string& filename, const std::string& outputFileName) {

        auto dictionary = initializeDecodingDictionary();

        bit_string toBeDecompressed = BinaryIO::readBitString(filename);

        std::string decoded;
        decoded.reserve(toBeDecompressed.size()/BYTE);
        uint32_t currentWordLength = numberOfBitsToStoreRangeOf(dictionary.size());
        for (int i = 0; i < toBeDecompressed.length(); i += currentWordLength) {

            // The left characters are not enough to create a word
            // i.e. they are garbage to align to bytes
            if (toBeDecompressed.length() - i < currentWordLength)
                break;

            currentWordLength = numberOfBitsToStoreRangeOf(dictionary.size() + 1);

            uint32_t index = toBeDecompressed.substr(i, currentWordLength).to_uint_32();

            // The last char in the previous entry is not added, as its value is the first char of the next entry
            if (i != 0) {
                uint8_t lastCharInPreviousEntry = dictionary[index].front();
                dictionary[dictionary.size() - 1] += lastCharInPreviousEntry;
            }

            std::string word = dictionary[index];
            decoded += word;
            dictionary[dictionary.size()] = std::move(word);

        }

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, decoded);
    }


};


#endif //LZW_H
