#ifndef BWT_H
#define BWT_H

#include "BinaryIO.h"
#include "SuffixArray.h"

/**
 * Burrows - Wheeler Transform
 */
class BWT {
    typedef basic_string<unsigned char> ustring;

    int originalIndex = 0;

public:

    void encode(const string &filename , const string &outputFileName) {

        remove(outputFileName.c_str()); // remove the file if exists

        ustring toBeEncoded = BinaryIO::readUnsignedBinaryFile(filename);

        vector<unsigned int> suffixArray = SuffixArray::buildSuffixArray(toBeEncoded);

        ustring bwt = generateBWT(toBeEncoded , suffixArray);

        BinaryIO::writeBinaryFile(outputFileName , bwt);

        BinaryIO::writeBinaryFile(outputFileName , originalIndex);
    }

private:

    // Generate Burrows - Wheeler Transform of given text
    ustring generateBWT(const ustring &input , vector<unsigned int> &suffixArray) {
        // Iterates over the suffix array to find
        // the last char of each cyclic rotation
        ustring bwtLastColumn;

        int n = input.length();
        for (int i = 0 ; i < suffixArray.size() ; ++i) {
            // Computes the last char which is given by
            // input[(suffix + n - 1) % n]
            int j = suffixArray[i] - 1;
            if (j < 0) {
                j += n;
            }

            if (suffixArray[i] == 0)
                originalIndex = i;

            bwtLastColumn += input[j];
        }

        return bwtLastColumn;
    }

