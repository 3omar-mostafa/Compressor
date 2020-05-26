#ifndef BINARY_IO_H
#define BINARY_IO_H

#include <fstream>
#include <string>
#include "Converter.h"

namespace BinaryIO {

    typedef std::basic_string<unsigned char> ustring;

    std::string readBinaryFile(const std::string& filename) {
        std::ifstream input(filename, std::ios::in | std::ios::binary);
        std::string fileData;
        if (input) {
            input.seekg(0, std::ios::end);
            fileData.resize(input.tellg());
            input.seekg(0, std::ios::beg);
            input.read(&fileData[0], fileData.size());
            input.close();
        }
        return fileData;
    }

    ustring readUnsignedBinaryFile(const std::string& filename) {
        std::ifstream input(filename, std::ios::in | std::ios::binary);
        ustring fileData;
        if (input) {
            input.seekg(0, std::ios::end);
            fileData.resize(input.tellg());
            input.seekg(0, std::ios::beg);
            input.read((char*) (&fileData[0]), fileData.size());
            input.close();
        }
        return fileData;
    }


    // Append binaryData to the end of the file for strings
    void writeBinaryFile(const std::string& filename, const std::string& binaryData) {

        std::ofstream output(filename, std::ios::out | std::ios::binary | std::ios::app);

        output.write(binaryData.c_str(), binaryData.length());

        output.close();
    }

    // Append binaryData to the end of the file for unsigned strings
    void writeBinaryFile(const std::string& filename, const ustring& binaryData) {

        std::ofstream output(filename, std::ios::out | std::ios::binary | std::ios::app);

        output.write((char*) binaryData.c_str(), binaryData.length());

        output.close();
    }

    // Append binaryData to the end of the file for integers
    void writeBinaryFile(const std::string& filename, int binaryData) {

        std::string data = Converter::int32_ToBitString(binaryData);
        data = Converter::bitString_ToRealBinary(data);

        std::ofstream output(filename, std::ios::out | std::ios::binary | std::ios::app);

        output.write(data.c_str(), data.length());

        output.close();
    }

}

#endif //BINARY_IO_H