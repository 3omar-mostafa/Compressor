#ifndef BWT_H
#define BWT_H

#include "../../Utils/BinaryIO.h"
#include "SuffixArray.h"
#include <algorithm>
#include <list>

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

        toBeEncoded += '\0';

        vector<unsigned int> suffixArray = SuffixArray::buildSuffixArray(toBeEncoded);

        ustring bwt = generateBWT(toBeEncoded , suffixArray);

        BinaryIO::writeBinaryFile(outputFileName , bwt);

        BinaryIO::writeBinaryFile(outputFileName , originalIndex);
    }

    void decode(const string &filename , const string &outputFileName) {

        ustring bwt = BinaryIO::readUnsignedBinaryFile(filename);
        remove(outputFileName.c_str()); // remove the file if exists

        string index;

        for (int i = 0 ; i < sizeof(int) ; ++i) {
            index.insert(0 , 1 , bwt.back());
            bwt.pop_back();
        }

        originalIndex = (int) Converter::string_ToInt64(index);

        ustring inverseBWT = BWT::invertBWT(bwt , originalIndex);

        inverseBWT.pop_back(); // remove '\0' which was added at encoding

        BinaryIO::writeBinaryFile(outputFileName , inverseBWT);

    }


private:
    static void computeLeftShift(list<int> &list , int index , int leftShift[]) {
        leftShift[index] = list.front();
        list.pop_front();
    }

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


    static ustring invertBWT(const ustring &BWT , long long x) {
        int length = BWT.length();
        ustring sortedBWT = BWT;
        int* leftShift = new int[length];

        // Sorts the characters of BWT[] alphabetically
        sort(sortedBWT.begin() , sortedBWT.end());

        // Array of pointers that act as head nodes
        // to linked lists created to compute leftShift[]
        list<int> arr[256]; // 256 is the length all symbols as we us byte it is 256 maximum

        // Takes each distinct character of BWT[] as head
        // of a linked list and appends to it the new node
        // whose data part contains index at which
        // character occurs in BWT[]
        for (int i = 0 ; i < length ; i++) {
            arr[BWT[i]].push_back(i);
        }

        // Takes each distinct character of sorted_arr[] as head
        // of a linked list and finds leftShift[]
        for (int i = 0 ; i < length ; i++) {
            computeLeftShift(arr[sortedBWT[i]] , i , leftShift);
        }

        ustring inverseBWT;

        // Decodes the bwt
        for (int i = 0 ; i < length ; i++) {
            x = leftShift[x];
            inverseBWT += BWT[x];
        }

        delete[] leftShift;

        return inverseBWT;
    }

};

#endif //BWT_H
