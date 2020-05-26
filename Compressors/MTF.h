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

    typedef std::basic_string<unsigned char> ustring;

    std::list<unsigned char> symbolsList;

public:

    void encode(const std::string& filename, const std::string& outputFileName) {

        ustring toBeEncoded = BinaryIO::readUnsignedBinaryFile(filename);
        remove(outputFileName.c_str()); // remove the file if exists

        generateSymbols();

        ustring encoded;

        for (unsigned char symbol : toBeEncoded) {
            auto ptr = getIndexOfValue(symbol);
            encoded += ptr.index;
            symbolsList.erase(ptr.iterator);
            symbolsList.push_front(symbol);
        }

        BinaryIO::writeBinaryFile(outputFileName, encoded);

    }

    void decode(const std::string& filename, const std::string& outputFileName) {

        ustring toBeDecoded = BinaryIO::readUnsignedBinaryFile(filename);
        remove(outputFileName.c_str()); // remove the file if exists

        generateSymbols();

        ustring decoded;
        for (unsigned char index : toBeDecoded) {
            auto ptr = getValueOfIndex(index);
            decoded += ptr.value;
            symbolsList.erase(ptr.iterator);
            symbolsList.push_front(ptr.value);
        }

        BinaryIO::writeBinaryFile(outputFileName, decoded);

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