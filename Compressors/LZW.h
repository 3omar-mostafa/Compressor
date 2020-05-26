#ifndef LZW_H
#define LZW_H

#include <string>
#include <unordered_map>
#include "../Utils/BinaryIO.h"
#include "../Utils/Converter.h"

// Number of bits to store number [n] numbers from 0 to n-1
#ifdef __GNUC__ // If GNU GCC Compiler
// It is better than log as it has constant time O(1)
#define numberOfBits(n) (n == 0 ? 0 : (n == 1) ? 1 : (32 - __builtin_clz(n-1)))
#else
#include <cmath>
#define numberOfBits(n) (n == 0 ? 0 : (n == 1) ? 1 : (int(log2(n-1)) + 1))
#endif
#define ONE_MEGA_BYTE (1024*1024*8)

class LZW {

    std::unordered_map<std::string, int> encodingDictionary;
    std::unordered_map<int, std::string> decodingDictionary;
    int dictionarySize;
    int currentWordLength;


    void initializeEncodingDictionary() {
        encodingDictionary.clear();
        for (int i = 0; i < 256; ++i) {
            std::string s(1, i);
            encodingDictionary[s] = i;
        }
        dictionarySize = encodingDictionary.size();
        currentWordLength = numberOfBits(dictionarySize);
    }


    void initializeDecodingDictionary() {
        decodingDictionary.clear();
        for (int i = 0; i < 256; ++i) {
            std::string s(1, i);
            decodingDictionary[i] = s;
        }
        dictionarySize = decodingDictionary.size();
        currentWordLength = numberOfBits(dictionarySize);
    }


public:

    void encode(const std::string& filename, const std::string& outputFileName) {

        initializeEncodingDictionary();

        std::string toBeCompressedString = BinaryIO::read(filename);

        remove(outputFileName.c_str()); // remove the file if exists

        std::string currentMatch, bitString, encodedData;
        for (char c : toBeCompressedString) {
            currentMatch += c;

            // if not found in encodingDictionary
            if (encodingDictionary.find(currentMatch) == encodingDictionary.end()) {
                encodingDictionary[currentMatch] = dictionarySize;
                dictionarySize++;
                currentMatch.pop_back();
                currentWordLength = numberOfBits(dictionarySize);
                bitString += Converter::bits_ToBitString(encodingDictionary[currentMatch], currentWordLength);
                currentMatch = c;
            }

            // Save the encoded data until now to free memory
            if (bitString.length() >= ONE_MEGA_BYTE) {
                int maxLengthFitInBytes = bitString.length() & (~(BYTE - 1u));
                encodedData = Converter::bitString_ToRealBinary(bitString, maxLengthFitInBytes);
                BinaryIO::write(outputFileName, encodedData);
                bitString.erase(0, maxLengthFitInBytes);
                encodedData.clear();
            }

        }

        // save last matched word
        currentWordLength = numberOfBits(dictionarySize + 1);
        bitString += Converter::bits_ToBitString(encodingDictionary[currentMatch], currentWordLength);

        encodedData = Converter::bitString_ToRealBinary(bitString);
        BinaryIO::write(outputFileName, encodedData);
        bitString.clear();

        encodingDictionary.clear();
    }

    void decode(const std::string& filename, const std::string& outputFileName) {

        initializeDecodingDictionary();

        std::string toBeDecompressed = BinaryIO::read(filename);

        remove(outputFileName.c_str()); // remove the file if exists

        toBeDecompressed = Converter::string_ToBitString(toBeDecompressed);

        std::string decoded;
        unsigned int index;
        for (int i = 0; i < toBeDecompressed.length(); i += currentWordLength) {

            // The left characters are not enough to create a word
            // i.e. they are garbage to align to bytes
            if (toBeDecompressed.length() - i < currentWordLength)
                break;

            currentWordLength = numberOfBits(dictionarySize + 1);

            index = Converter::bitString_ToInt(toBeDecompressed.substr(i, currentWordLength));
            char lastCharInPreviousEntry = decodingDictionary[index][0];

            if (i != 0)
                decodingDictionary[dictionarySize - 1] += lastCharInPreviousEntry;

            std::string out = decodingDictionary[index];
            decoded += out;
            decodingDictionary[dictionarySize] = out;

            dictionarySize++;

        }

        BinaryIO::write(outputFileName, decoded);
        decodingDictionary.clear();

    }


};


#endif //LZW_H
