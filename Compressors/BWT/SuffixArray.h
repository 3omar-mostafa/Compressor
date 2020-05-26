#ifndef SUFFIX_ARRAY_H
#define SUFFIX_ARRAY_H

#include <algorithm>
#include <vector>
#include <string>


// Structure to store information of a suffix
struct suffix {
    unsigned int index; // To store original index
    unsigned int rank[2]; // To store ranks and next rank pair

    bool operator <(const suffix& rhs) const {
        return (this->rank[0] == rhs.rank[0]) ?
               (this->rank[1] < rhs.rank[1] ? 1 : 0) :
               (this->rank[0] < rhs.rank[0] ? 1 : 0);
    }

};

/**
 * Class for building suffix array of a given unsigned string
 */
class SuffixArray {

public:

    static std::vector<unsigned int> buildSuffixArray(const std::string& inputString) {
        int length = inputString.length();

        auto* suffixes = new suffix[length];

        // Store suffixes and their indexes in an array of structures.
        // The structure is needed to sort the suffixes lexicographically
        // and maintain their old indexes while sorting
        for (int i = 0; i < length; i++) {
            suffixes[i].index = i;
            suffixes[i].rank[0] = inputString[i];
            suffixes[i].rank[1] = ((i + 1) < length) ? (inputString[i + 1]) : UINT32_MAX;
        }

        std::sort(suffixes, suffixes + length);

        // At this point, all suffixes are sorted according to first
        // 2 characters. Let us sort suffixes according to first 4
        // characters, then first 8 and so on
        auto* ind = new unsigned int[length]; // This array is needed to get the index in suffixes[]
        // from original index. This mapping is needed to get
        // next suffix.
        for (int k = 4; k < 2 * length; k = k * 2) {
            // Assigning rank and index values to first suffix
            int rank = 0;
            int prev_rank = suffixes[0].rank[0];
            suffixes[0].rank[0] = rank;
            ind[suffixes[0].index] = 0;

            // Assigning rank to suffixes
            for (int i = 1; i < length; i++) {
                // If first rank and next ranks are same as that of previous
                // suffix in array, assign the same new rank to this suffix
                if (suffixes[i].rank[0] == prev_rank &&
                    suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                    prev_rank = suffixes[i].rank[0];
                    suffixes[i].rank[0] = rank;
                } else // Otherwise increment rank and assign
                {
                    prev_rank = suffixes[i].rank[0];
                    suffixes[i].rank[0] = ++rank;
                }
                ind[suffixes[i].index] = i;
            }

            // Assign next rank to every suffix
            for (int i = 0; i < length; i++) {
                unsigned int nextIndex = suffixes[i].index + k / 2;
                suffixes[i].rank[1] = (nextIndex < length) ? suffixes[ind[nextIndex]].rank[0] : -1;
            }

            // Sort the suffixes according to first k characters
            std::sort(suffixes, suffixes + length);
        }

        delete[] ind;

        // Store indexes of all sorted suffixes in the suffix array
        std::vector<unsigned int> suffixArr(length);
        for (int i = 0; i < length; i++)
            suffixArr[i] = suffixes[i].index;

        delete[] suffixes;
        // Return the suffix array
        return suffixArr;
    }
};


#endif //SUFFIX_ARRAY_H
