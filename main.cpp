#include <iostream>
#include "Compressors/LZW.h"
#include "Compressors/BWT/BWT.h"
#include "Compressors/MTF.h"

void compress(const string& toBeCompressedFilename, const string& outputFilename);

void decompress(const string& toBeDecompressedFilename, const string& outputFilename);

using namespace std;


int main() {

    cout << "Welcome to Compressor!\n";
    cout << "_______________________________________\n";
    cout << "Choose a mode : a) encode     b) decode \n";

    string mode;

    while (true) {
        cin >> mode;
        if (mode == "a" || mode == "encode" || mode == "e" || mode == "b" || mode == "decode" || mode == "d")
            break;
        cout << "Please enter a valid mode" << endl
             << "Type 'a' or 'encode' or 'e' [without quotes] to select encoding for example" << endl;
    }

    if (mode == "a" || mode == "encode" || mode == "e") {
        string toBeCompressedFilename, outputFilename;

        cout << "Please enter path to the file you want to encode:\n";
        cin >> toBeCompressedFilename;
        cout << "Please enter path to the output file including its name and extension:\n";
        cin >> outputFilename;

        compress(toBeCompressedFilename, outputFilename);

        cout << "Finished encoding\nThank you";
    } else {

        string toBeDecompressedFilename, outputFilename;

        cout << "Please enter path to the file you want to decode:\n";
        cin >> toBeDecompressedFilename;
        cout << "Please enter path to the output file including its name and extension:\n";
        cin >> outputFilename;

        decompress(toBeDecompressedFilename, outputFilename);

        cout << "Finished decoding\nThank you";
    }

    return 0;
}

void compress(const string& toBeCompressedFilename, const string& outputFilename) {

    BWT bwt;
    bwt.encode(toBeCompressedFilename, outputFilename);

    MTF mtf;
    mtf.encode(outputFilename, outputFilename); // same filename will makes it write output to the same file

    LZW lzw;
    lzw.encode(outputFilename, outputFilename); // same filename will makes it write output to the same file

}


void decompress(const string& toBeDecompressedFilename, const string& outputFilename) {

    LZW lzw;
    lzw.decode(toBeDecompressedFilename, outputFilename);

    MTF mtf;
    mtf.decode(outputFilename, outputFilename); // same filename will makes it write output to the same file

    BWT bwt;
    bwt.decode(outputFilename, outputFilename); // same filename will makes it write output to the same file

}

