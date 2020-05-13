#ifndef BINARY_IO_H
#define BINARY_IO_H

#include <fstream>
#include <string>

using namespace std;

namespace BinaryIO {

    string readBinaryFile(const string& filename) {
        ifstream input(filename, ios::in | ios::binary);
        string fileData;
        if (input) {
            input.seekg(0, ios::end);
            fileData.resize(input.tellg());
            input.seekg(0, ios::beg);
            input.read(&fileData[0], fileData.size());
            input.close();
        }
        return fileData;
    }

    // Append binaryData to the end of the file for strings
    void writeBinaryFile(const string& filename, const string& binaryData) {

        ofstream output(filename, ios::out | ios::binary | ios::app);

        output.write(binaryData.c_str(), binaryData.length());

        output.close();
    }

}

#endif //BINARY_IO_H
