#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <string>
#include "BWT/BWT.h"
#include "MTF.h"
#include "LZW/LZW.h"

namespace Compressor {

    void compress(const std::string& toBeCompressedFilename, const std::string& outputFilename) {
        BWT::encode(toBeCompressedFilename, outputFilename);
        MTF::encode(outputFilename, outputFilename); // same filename will makes it write output to the same file
        LZW::encode(outputFilename, outputFilename); // same filename will makes it write output to the same file
    }


    void decompress(const std::string& toBeDecompressedFilename, const std::string& outputFilename) {
        LZW::decode(toBeDecompressedFilename, outputFilename);
        MTF::decode(outputFilename, outputFilename); // same filename will makes it write output to the same file
        BWT::decode(outputFilename, outputFilename); // same filename will makes it write output to the same file
    }

}

#endif //COMPRESSOR_H
