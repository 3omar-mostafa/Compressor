#ifndef CONVERTER_H
#define CONVERTER_H

#include <string>
#include <bitset>
#include <stdexcept>
#include <cstdint>


namespace Converter {

    static const uint32_t BYTE = 8;

    std::string string_ToBitString(const std::string& toBeConverted) {
        std::string bitString;
        for (const char c : toBeConverted) {
            bitString += std::bitset<BYTE>(c).to_string();
        }
        return bitString;
    }

    std::string string_ToBitString(const std::string& toBeConverted, int startPosition, int length) {
        int endPosition = startPosition + length;

        std::string bitString;
        for (int i = startPosition; i < endPosition && i < toBeConverted.length(); ++i) {
            bitString += std::bitset<BYTE>(toBeConverted[i]).to_string();
        }
        return bitString;
    }


    uint64_t string_ToInt64(const std::string& toBeConverted) {
        if (toBeConverted.length() > 8)
            throw std::invalid_argument("toBeConverted length is bigger than 8 which max size for numbers in bytes");

        std::string bitString;
        for (char c : toBeConverted) {
            bitString += std::bitset<BYTE>(c).to_string();
        }

        return std::bitset<8 * BYTE>(bitString).to_ullong();
    }

    std::string bitString_ToRealBinary(const std::string& toBeConverted) {
        std::string binaryString;

        for (int i = 0; i < toBeConverted.length(); i += BYTE) {
            std::bitset<BYTE> bitset(toBeConverted, i, BYTE);
            binaryString += (uint8_t) bitset.to_ulong();
        }


        if (toBeConverted.length() % BYTE != 0) {
            uint32_t size = BYTE - toBeConverted.length() % BYTE;
            char last = binaryString.back() << size;
            binaryString.pop_back();
            binaryString.push_back(last);
        }

        return binaryString;
    }

    std::string bitString_ToRealBinary(const std::string& toBeConverted, int startPosition, int length) {

        int endPosition = startPosition + length;

        std::string binaryString;

        for (int i = startPosition; i < endPosition && i < toBeConverted.length(); i += BYTE) {
            std::bitset<BYTE> bitset(toBeConverted, i, BYTE);
            binaryString += (uint8_t) bitset.to_ulong();
        }

        return binaryString;
    }

    std::string bitString_ToRealBinary(const std::string& toBeConverted, int length) {
        return bitString_ToRealBinary(toBeConverted, 0, length);
    }

    std::string char_ToBitString(const char toBeConverted) {
        return std::bitset<BYTE>(toBeConverted).to_string();
    }

    std::string int8_ToBitString(uint8_t toBeConverted) {
        return char_ToBitString(toBeConverted);
    }

    std::string int16_ToBitString(uint16_t toBeConverted) {
        return std::bitset<2 * BYTE>(toBeConverted).to_string();
    }

    std::string int32_ToBitString(uint32_t toBeConverted) {
        return std::bitset<4 * BYTE>(toBeConverted).to_string();
    }

    // Convert the numberOfBits left most bits in toBeConverted to bit string, i.e. '0' , '1' string
    std::string bits_ToBitString(uint64_t toBeConverted, int noOfBits) {
        std::string bitString = std::bitset<8 * BYTE>(toBeConverted).to_string();
        bitString.erase(0, 64 - noOfBits);
        return bitString;
    }

    uint32_t bitString_ToInt(const std::string& toBeConverted) {
        return std::bitset<4 * BYTE>(toBeConverted).to_ulong();
    }

}

#endif // CONVERTER_H