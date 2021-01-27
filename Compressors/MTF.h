#ifndef MTF_H
#define MTF_H

#include <list>
#include <string>
#include <cstdint>
#include "../Utils/BinaryIO.h"

/**
 * Wrapper around std::list::iterator
 */
struct Iterator {
    std::list<uint8_t>::const_iterator iterator;
    uint8_t index;
    uint8_t value;

    Iterator(const std::list<uint8_t>::const_iterator& iterator, uint32_t index) {
        this->iterator = iterator;
        this->index = index;
        this->value = *iterator;
    }
};


/**
 * Move To Front Algorithm
 */
class MTF {

    static std::list<uint8_t> symbolsList;

public:

    static void encode(const std::string& filename, const std::string& outputFileName) {

        std::string toBeEncoded = BinaryIO::readString(filename);

        generateSymbols();

        for (auto& symbol : toBeEncoded) {
            auto ptr = getIndexOfValue(symbol);
            symbol = ptr.index; // Encode in-place
            symbolsList.erase(ptr.iterator);
            symbolsList.push_front(ptr.value);
        }

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, toBeEncoded);

    }

    static void decode(const std::string& filename, const std::string& outputFileName) {

        std::string toBeDecoded = BinaryIO::readString(filename);

        generateSymbols();

        for (auto& byte : toBeDecoded) {
            uint8_t index = byte;
            auto ptr = getValueOfIndex(index);
            byte = ptr.value; // Decode in-place
            symbolsList.erase(ptr.iterator);
            symbolsList.push_front(ptr.value);
        }

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, toBeDecoded);

    }

private:

    static void generateSymbols() {
        symbolsList.clear();
        for (uint32_t i = 0; i < 256; ++i) {
            symbolsList.push_back(i);
        }
    }

    static Iterator getIndexOfValue(uint8_t c) {
        uint32_t index = 0;
        for (auto iter = symbolsList.begin(); iter != symbolsList.end(); ++iter) {
            if (*iter == c)
                return {iter, index};
            index++;
        }
        return {std::list<uint8_t>::const_iterator(), 0};
    }

    static Iterator getValueOfIndex(uint32_t requiredIndex) {
        uint32_t index = 0;
        for (auto iter = symbolsList.begin(); iter != symbolsList.end(); ++iter) {
            if (index == requiredIndex)
                return {iter, index};
            index++;
        }
        return {std::list<uint8_t>::const_iterator(), 0};
    }


};

std::list<uint8_t> MTF::symbolsList;

#endif //MTF_H