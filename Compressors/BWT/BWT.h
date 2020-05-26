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

    int originalIndex = 0;

public:

    void encode(const std::string& filename, const std::string& outputFileName) {

        remove(outputFileName.c_str()); // remove the file if exists

        std::string toBeEncoded = BinaryIO::read(filename);

        toBeEncoded += '\0';

        std::vector<unsigned int> suffixArray = SuffixArray::buildSuffixArray(toBeEncoded);

        std::string bwt = generateBWT(toBeEncoded, suffixArray);

        BinaryIO::write(outputFileName, bwt);

        BinaryIO::write(outputFileName, originalIndex);
    }

    void decode(const std::string& filename, const std::string& outputFileName) {

        remove(outputFileName.c_str()); // remove the file if exists
        std::string bwt = BinaryIO::read(filename);

        std::string index;

        for (int i = 0; i < sizeof(originalIndex); ++i) {
            index.insert(0, 1, bwt.back());
            bwt.pop_back();
        }

        originalIndex = (int) Converter::string_ToInt64(index);

        std::string inverseBWT = BWT::invertBWT(bwt, originalIndex);

        inverseBWT.pop_back(); // remove '\0' which was added at encoding

        BinaryIO::write(outputFileName, inverseBWT);

    }


private:
    static void computeLeftShift(std::list<int>& list, int index, int leftShift[]) {
        leftShift[index] = list.front();
        list.pop_front();
    }

    // Generate Burrows - Wheeler Transform of given text
    std::string generateBWT(const std::string& input, std::vector<unsigned int>& suffixArray) {
        // Iterates over the suffix array to find
        // the last char of each cyclic rotation
        std::string bwtLastColumn;

        int n = input.length();
        for (int i = 0; i < suffixArray.size(); ++i) {
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

    static bool unsignedCharsCompare(const char c1, const char c2) {
        return (unsigned char) c1 < (unsigned char) c2;
    }

    static std::string invertBWT(const std::string& BWT, long long index) {
        int length = BWT.length();
        std::string sortedBWT = BWT;
        int* leftShift = new int[length];

        // Sorts the characters of BWT[] alphabetically
        std::sort(sortedBWT.begin(), sortedBWT.end(), unsignedCharsCompare);

        // Array of pointers that act as head nodes
        // to linked lists created to compute leftShift[]
        std::list<int> arr[256]; // 256 is the length all symbols as we us byte it is 256 maximum

        // Takes each distinct character of BWT[] as head
        // of a linked list and appends to it the new node
        // whose data part contains index at which
        // character occurs in BWT[]
        for (int i = 0; i < length; i++) {
            arr[(unsigned char) BWT[i]].push_back(i);
        }

        // Takes each distinct character of sorted_arr[] as head
        // of a linked list and finds leftShift[]
        for (int i = 0; i < length; i++) {
            computeLeftShift(arr[(unsigned char) sortedBWT[i]], i, leftShift);
        }

        std::string inverseBWT;

        // Decodes the bwt
        for (int i = 0; i < length; i++) {
            index = leftShift[index];
            inverseBWT += BWT[index];
        }

        delete[] leftShift;

        return inverseBWT;
    }

};

#endif //BWT_H
