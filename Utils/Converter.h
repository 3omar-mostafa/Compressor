#ifndef CONVERTER_H
#define CONVERTER_H

#include <string>
#include <bitset>
#include <stdexcept>

#define BYTE 8

using namespace std;

namespace Converter {
    
    string string_ToBitString(const string &toBeConverted) {
        string bitString;
        for (const char c : toBeConverted) {
            bitString += bitset<BYTE>(c).to_string();
        }
        return bitString;
    }
    
    string string_ToBitString(const string &toBeConverted , int startPosition , int length) {
        int endPosition = startPosition + length;
        
        string bitString;
        for (int i = startPosition ; i < endPosition && i < toBeConverted.length() ; ++i) {
            bitString += bitset<BYTE>(toBeConverted[i]).to_string();
        }
        return bitString;
    }
    

    unsigned long long string_ToInt64(const string &toBeConverted) {
        if (toBeConverted.length() > 8)
            throw invalid_argument("toBeConverted length is bigger than 8 which max size for numbers in bytes");

        string bitString;
        for (const char c : toBeConverted) {
            bitString += bitset<BYTE>(c).to_string();
        }

        return bitset<8 * BYTE>(bitString).to_ullong();
    }

    string bitString_ToRealBinary(const string &toBeConverted) {
        string binaryString;
        
        for (int i = 0 ; i < toBeConverted.length() ; i += BYTE) {
            bitset<BYTE> bitset(toBeConverted , i , BYTE);
            binaryString += (unsigned char) bitset.to_ulong();
        }
        
        
        if (toBeConverted.length() % BYTE != 0) {
            unsigned int size = BYTE - toBeConverted.length() % BYTE;
            char last = binaryString.back() << size;
            binaryString.pop_back();
            binaryString.push_back(last);
        }
        
        return binaryString;
    }
    
    string bitString_ToRealBinary(const string &toBeConverted , int startPosition , int length) {
        
        int endPosition = startPosition + length;
        
        string binaryString;
        
        for (int i = startPosition ; i < endPosition && i < toBeConverted.length() ; i += BYTE) {
            bitset<BYTE> bitset(toBeConverted , i , BYTE);
            binaryString += (unsigned char) bitset.to_ulong();
        }
        
        return binaryString;
    }
    
    string bitString_ToRealBinary(const string &toBeConverted , int length) {
        return bitString_ToRealBinary(toBeConverted , 0 , length);
    }
    
    string char_ToBitString(const char toBeConverted) {
        return bitset<BYTE>(toBeConverted).to_string();
    }
    
    string int8_ToBitString(const unsigned char toBeConverted) {
        return char_ToBitString(toBeConverted);
    }
    
    string int16_ToBitString(const short toBeConverted) {
        return bitset<2 * BYTE>(toBeConverted).to_string();
    }

    string int32_ToBitString(const int toBeConverted) {
        return bitset<4 * BYTE>(toBeConverted).to_string();
    }

    // Convert the numberOfBits left most bits in toBeConverted to bit string, i.e. '0' , '1' string
    string bits_ToBitString(unsigned long long toBeConverted , int noOfBits) {
        string bitString = bitset<8 * BYTE>(toBeConverted).to_string();
        bitString.erase(0 , 64 - noOfBits);
        return bitString;
    }

    unsigned int bitString_ToInt(const string &toBeConverted) {
        return bitset<4 * BYTE>(toBeConverted).to_ulong();
    }
    
}

#endif // CONVERTER_H