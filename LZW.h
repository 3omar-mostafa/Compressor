#ifndef LZW_H
#define LZW_H

#include <string>
#include <unordered_map>
#include "BinaryIO.h"
#include "Converter.h"

// Number of bits to store number [n] numbers from 0 to n-1
#ifdef __GNUC__ // If GNU GCC Compiler
    // It is better than log as it has constant time O(1)
    #define numberOfBits(n) (n == 0 ? 0 : (n == 1) ? 1 : (32 - __builtin_clz(n-1)))
#else
    #include <cmath>
    #define numberOfBits(n) (n == 0 ? 0 : (n == 1) ? 1 : (int(log2(n-1)) + 1))
#endif
#define ONE_MEGA_BYTE (1024*1024*8)

using namespace std;

class LZW {

    unordered_map<string , int> dictionary;
    int dictionarySize;
    int currentWordLength;


    void initializeEncodingDictionary() {
        dictionary.clear();
        for (int i = 0 ; i < 256 ; ++i) {
            string s(1 , i);
            dictionary[s] = i;
        }
        dictionarySize = dictionary.size();
        currentWordLength = numberOfBits(dictionarySize);
    }

public:

    void encode(const string &filename , const string &outputFileName) {

        initializeEncodingDictionary();

        string toBeCompressedString = BinaryIO::readBinaryFile(filename);

        remove(outputFileName.c_str()); // remove the file if exists

        string currentMatch , bitString , encodedData;
        for (char c : toBeCompressedString) {
            currentMatch += c;

            // if not found in dictionary
            if (dictionary.find(currentMatch) == dictionary.end()) {
                dictionary[currentMatch] = dictionarySize;
                dictionarySize++;
                currentMatch.pop_back();
                currentWordLength = numberOfBits(dictionarySize);
                bitString += Converter::bits_ToBitString(dictionary[currentMatch] , currentWordLength);
                currentMatch = c;
            }

            // Save the encoded data until now to free memory
            if (bitString.length() >= ONE_MEGA_BYTE) {
                int maxLengthFitInBytes = bitString.length() & (~(BYTE - 1u));
                encodedData = Converter::bitString_ToRealBinary(bitString , maxLengthFitInBytes);
                BinaryIO::writeBinaryFile(outputFileName , encodedData);
                bitString.erase(0 , maxLengthFitInBytes);
                encodedData.clear();
            }

        }

        // save last matched word
        currentWordLength = numberOfBits(dictionarySize + 1);
        bitString += Converter::bits_ToBitString(dictionary[currentMatch] , currentWordLength);

        encodedData = Converter::bitString_ToRealBinary(bitString);
        BinaryIO::writeBinaryFile(outputFileName , encodedData);
        bitString.clear();

        dictionary.clear();
    }
};


#endif //LZW_H