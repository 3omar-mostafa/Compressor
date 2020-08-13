#include <iostream>
#include "Compressors/LZW/LZW.h"
#include "Compressors/BWT/BWT.h"
#include "Compressors/MTF.h"

void compress(const std::string& toBeCompressedFilename, const std::string& outputFilename);

void decompress(const std::string& toBeDecompressedFilename, const std::string& outputFilename);

int main() {

    std::cout << "Welcome to Compressor!\n";
    std::cout << "_______________________________________\n";
    std::cout << "Choose a mode : a) encode     b) decode \n";

    std::string mode;

    while (true) {
        std::cin >> mode;
        if (mode == "a" || mode == "encode" || mode == "e" || mode == "b" || mode == "decode" || mode == "d")
            break;
        std::cout << "Please enter a valid mode" << std::endl
                  << "Type 'a' or 'encode' or 'e' [without quotes] to select encoding for example" << std::endl;
    }

    if (mode == "a" || mode == "encode" || mode == "e") {
        std::string toBeCompressedFilename, outputFilename;

        std::cout << "Please enter path to the file you want to encode:\n";
        std::cin >> toBeCompressedFilename;
        std::cout << "Please enter path to the output file including its name and extension:\n";
        std::cin >> outputFilename;

        compress(toBeCompressedFilename, outputFilename);

        std::cout << "Finished encoding\nThank you";
    } else {

        std::string toBeDecompressedFilename, outputFilename;

        std::cout << "Please enter path to the file you want to decode:\n";
        std::cin >> toBeDecompressedFilename;
        std::cout << "Please enter path to the output file including its name and extension:\n";
        std::cin >> outputFilename;

        decompress(toBeDecompressedFilename, outputFilename);

        std::cout << "Finished decoding\nThank you";
    }

    return 0;
}

void compress(const std::string& toBeCompressedFilename, const std::string& outputFilename) {

    BWT bwt;
    bwt.encode(toBeCompressedFilename, outputFilename);

    MTF mtf;
    mtf.encode(outputFilename, outputFilename); // same filename will makes it write output to the same file

    LZW::encode(outputFilename, outputFilename); // same filename will makes it write output to the same file

}


void decompress(const std::string& toBeDecompressedFilename, const std::string& outputFilename) {

    LZW::decode(toBeDecompressedFilename, outputFilename);

    MTF mtf;
    mtf.decode(outputFilename, outputFilename); // same filename will makes it write output to the same file

    BWT bwt;
    bwt.decode(outputFilename, outputFilename); // same filename will makes it write output to the same file

}

