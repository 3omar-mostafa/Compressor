#include <iostream>
#include "Compressors/Compressor.h"

void compressWithOutputInfo(const std::string& toBeCompressedFilename, const std::string& outputFilename);

void decompressWithOutputInfo(const std::string& toBeDecompressedFilename, const std::string& outputFilename);

void checkFilesAndExitIfFoundErrors(const std::string& inputFilename, const std::string& outputFilename);

void showHelp();

int main(int argc, char** argv) {

    if (argc == 4) {
        std::string mode = argv[1];
        if (mode == "-c" || mode == "--compress") {
            std::string toBeCompressedFilename = argv[2];
            std::string outputFilename = argv[3];
            compressWithOutputInfo(toBeCompressedFilename, outputFilename);
        } else if (mode == "-d" || mode == "--decompress") {
            std::string toBeDecompressedFilename = argv[2];
            std::string outputFilename = argv[3];
            decompressWithOutputInfo(toBeDecompressedFilename, outputFilename);
        } else {
            showHelp();
        }
    } else {
        showHelp();
    }
    return 0;
}

void decompressWithOutputInfo(const std::string& toBeDecompressedFilename, const std::string& outputFilename) {

    checkFilesAndExitIfFoundErrors(toBeDecompressedFilename, outputFilename);

    std::cout << "Decompressing...\n";
    Compressor::decompress(toBeDecompressedFilename, outputFilename);
    std::cout << "Finished Decompressing\n";
}


void compressWithOutputInfo(const std::string& toBeCompressedFilename, const std::string& outputFilename) {

    checkFilesAndExitIfFoundErrors(toBeCompressedFilename, outputFilename);

    std::cout << "Compressing...\n";
    Compressor::compress(toBeCompressedFilename, outputFilename);
    std::cout << "Finished Compressing\n";

    int originalFileSize = BinaryIO::getFileSize(toBeCompressedFilename);
    int compressedFileSize = BinaryIO::getFileSize(outputFilename);
    std::cout << "Compression Ratio: " << 1.0 * originalFileSize / compressedFileSize << std::endl;
}

void checkFilesAndExitIfFoundErrors(const std::string& inputFilename, const std::string& outputFilename) {
    if (!BinaryIO::doesFileExist(inputFilename)) {
        std::cerr << "File " << inputFilename << " is not found!\n";
        exit(0);
    }

    if (BinaryIO::doesFileExist(outputFilename)) {
        std::cout << outputFilename << " already exists, Do you want to overwrite it? [Y]/[N] : ";
        std::string answer;
        std::cin >> answer;
        if (answer != "y" && answer != "Y") {
            exit(0);
        }
    }
}


void showHelp() {
    std::cout << "Welcome to Compressor!\n"
                 "________________________________________________\n"
                 "Usage : compressor OPTION input_file output_file\n\n"

                 "OPTION:\n"
                 "        -c  --compress     Compress the file\n"
                 "        -d  --decompress   Decompress the file\n\n";

}
