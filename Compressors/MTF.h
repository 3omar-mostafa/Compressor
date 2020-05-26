#ifndef MTF_H
#define MTF_H

#include <list>
#include <string>
#include "../Utils/BinaryIO.h"

/**
 * Wrapper around std::list::iterator
 */
struct Iterator {
    std::list<unsigned char>::const_iterator iterator;
    unsigned char index;
    unsigned char value;

    Iterator(const std::list<unsigned char>::const_iterator& iterator, unsigned int index) {
        this->iterator = iterator;
        this->index = index;
        this->value = *iterator;
    }
};


/**
 * Move To Front Algorithm
 */
class MTF {

    std::list<unsigned char> symbolsList;

public:

    void encode(const std::string& filename, const std::string& outputFileName) {

        std::string toBeEncoded = BinaryIO::read(filename);
        remove(outputFileName.c_str()); // remove the file if exists

        generateSymbols();

        std::string encoded;

        for (unsigned char symbol : toBeEncoded) {
            auto ptr = getIndexOfValue(symbol);
            encoded += ptr.index;
            symbolsList.erase(ptr.iterator);
            symbolsList.push_front(symbol);
        }

        BinaryIO::write(outputFileName, encoded);

    }

    void decode(const std::string& filename, const std::string& outputFileName) {

        std::string toBeDecoded = BinaryIO::read(filename);
        remove(outputFileName.c_str()); // remove the file if exists

        generateSymbols();

        std::string decoded;
        for (unsigned char index : toBeDecoded) {
            auto ptr = getValueOfIndex(index);
            decoded += ptr.value;
            symbolsList.erase(ptr.iterator);
            symbolsList.push_front(ptr.value);
        }

        BinaryIO::write(outputFileName, decoded);

    }

private:

    void generateSymbols() {
        for (unsigned int i = 0; i < 256; ++i) {
            symbolsList.push_back(i);
        }
    }

    Iterator getIndexOfValue(unsigned char c) {
        unsigned int index = 0;
        for (auto iter = symbolsList.begin(); iter != symbolsList.end(); ++iter) {
            if (*iter == c)
                return {iter, index};
            index++;
        }
        return {std::list<unsigned char>::const_iterator(), 0};
    }

    Iterator getValueOfIndex(unsigned int requiredIndex) {
        unsigned int index = 0;
        for (auto iter = symbolsList.begin(); iter != symbolsList.end(); ++iter) {
            if (index == requiredIndex)
                return {iter, index};
            index++;
        }
        return {std::list<unsigned char>::const_iterator(), 0};
    }


};


#endif //MTF_H