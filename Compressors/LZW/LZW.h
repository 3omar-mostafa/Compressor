#ifndef LZW_H
#define LZW_H

#include <string>
#include <unordered_map>
#include "../../Utils/BinaryIO.h"
#include "../../Utils/Converter.h"
#include "Utils.h"


class LZW {

    static std::unordered_map<std::string, int> initializeEncodingDictionary() {
        std::unordered_map<std::string, int> encodingDictionary;
        for (int i = 0; i < 256; ++i) {
            std::string s(1, i);
            encodingDictionary[s] = i;
        }
        return encodingDictionary;
    }

    static std::unordered_map<int, std::string> initializeDecodingDictionary() {
        std::unordered_map<int, std::string> decodingDictionary;
        for (int i = 0; i < 256; ++i) {
            std::string s(1, i);
            decodingDictionary[i] = s;
        }
        return decodingDictionary;
    }

public:


    static void encode(const std::string& filename, const std::string& outputFileName) {

        auto dictionary = initializeEncodingDictionary();
        unsigned int currentWordLength;

        std::string toBeCompressedString = BinaryIO::read(filename);

        std::string currentMatch, bitString;
        for (char c : toBeCompressedString) {
            currentMatch += c;

            // if not found in dictionary
            if (dictionary.find(currentMatch) == dictionary.end()) {
                dictionary[currentMatch] = dictionary.size();
                currentWordLength = numberOfBitsToStoreRangeOf(dictionary.size());
                currentMatch.pop_back();
                bitString += Converter::bits_ToBitString(dictionary[currentMatch], currentWordLength);
                currentMatch = c;
            }

        }

        // save last matched word
        currentWordLength = numberOfBitsToStoreRangeOf(dictionary.size());
        bitString += Converter::bits_ToBitString(dictionary[currentMatch], currentWordLength);

        std::string encodedData = Converter::bitString_ToRealBinary(bitString);
        BinaryIO::write(outputFileName, encodedData);
    }


    static void decode(const std::string& filename, const std::string& outputFileName) {

        auto dictionary = initializeDecodingDictionary();

        std::string toBeDecompressed = Converter::string_ToBitString(BinaryIO::read(filename));

        std::string decoded;
        unsigned int index;
        unsigned int currentWordLength = numberOfBitsToStoreRangeOf(dictionary.size());
        for (int i = 0; i < toBeDecompressed.length(); i += currentWordLength) {

            // The left characters are not enough to create a word
            // i.e. they are garbage to align to bytes
            if (toBeDecompressed.length() - i < currentWordLength)
                break;

            currentWordLength = numberOfBitsToStoreRangeOf(dictionary.size() + 1);

            index = Converter::bitString_ToInt(toBeDecompressed.substr(i, currentWordLength));

            // The last char in the previous entry is not added, as its value is the first char of the next entry
            if (i != 0) {
                char lastCharInPreviousEntry = dictionary[index][0];
                dictionary[dictionary.size() - 1] += lastCharInPreviousEntry;
            }

            std::string out = dictionary[index];
            decoded += out;
            dictionary[dictionary.size()] = out;

        }

        BinaryIO::write(outputFileName, decoded);
    }


};


#endif //LZW_H
