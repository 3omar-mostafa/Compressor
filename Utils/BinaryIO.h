#ifndef BINARY_IO_H
#define BINARY_IO_H

#include <fstream>
#include <string>
#include "Converter.h"
#include <bit_string.h>

namespace BinaryIO {

    static const uint32_t BYTE = 8;

    bool doesFileExist(const std::string& filename) {
        std::ifstream input(filename, std::ios::in | std::ios::binary);
        return input.good();
    }

    int getFileSize(std::ifstream& input) {
        input.seekg(0, std::ios::end);
        return input.tellg();
    }

    int getFileSize(const std::string& filename) {
        std::ifstream input(filename);
        return getFileSize(input);
    }

    std::string readString(const std::string& filename, int startPosition, int length) {
        std::ifstream input(filename, std::ios::in | std::ios::binary);
        std::string fileData;
        if (input) {
            input.seekg(startPosition);
            fileData.resize(length);
            input.read(&fileData[0], fileData.size());
            input.close();
        }
        return fileData;
    }

    std::string readString(const std::string& filename, int startPosition) {
        return readString(filename, startPosition, getFileSize(filename) - startPosition);
    }

    std::string readString(const std::string& filename) {
        return readString(filename, 0, getFileSize(filename));
    }

    bit_string readBitString(const std::string& filename, int startPosition, int length) {
        std::ifstream input(filename, std::ios::in | std::ios::binary);
        bit_string fileData;
        if (input) {
            input.seekg(startPosition);
            fileData.resize(length * BYTE);
            input.read((char*) fileData.data(), fileData.length_in_bytes());
            input.close();
        }
        return fileData;
    }

    bit_string readBitString(const std::string& filename, int startPosition) {
        return readBitString(filename, startPosition, getFileSize(filename) - startPosition);
    }

    bit_string readBitString(const std::string& filename) {
        return readBitString(filename, 0, getFileSize(filename));
    }


    // Append binaryData to the end of the file for strings
    void write(const std::string& filename, const std::string& binaryData) {

        std::ofstream output(filename, std::ios::out | std::ios::binary | std::ios::app);

        output.write(binaryData.c_str(), binaryData.length());

        output.close();
    }

    // Append binaryData to the end of the file for strings
    void write(const std::string& filename, const bit_string& binaryData) {
        std::ofstream output(filename, std::ios::out | std::ios::binary | std::ios::app);
        output.write((char*) binaryData.data(), binaryData.length_in_bytes());
        output.close();
    }

    // Append binaryData to the end of the file for integers
    void write(const std::string& filename, int binaryData) {

        std::string data = Converter::int32_ToBitString(binaryData);
        data = Converter::bitString_ToRealBinary(data);

        std::ofstream output(filename, std::ios::out | std::ios::binary | std::ios::app);

        output.write(data.c_str(), data.length());

        output.close();
    }

}

#endif //BINARY_IO_H
