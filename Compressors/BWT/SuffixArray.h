#ifndef SUFFIX_ARRAY_H
#define SUFFIX_ARRAY_H

#include <algorithm>
#include <vector>
#include <string>


/**
 * Class for building suffix array of a given string
 *
 * @cite Juha Kärkkäinen, Peter Sanders, Stefan Burkhardt <br>
 * Linear Work Suffix Array Construction (2006).
 */
class SuffixArray {

public:

    static std::vector<unsigned int> buildSuffixArray(const std::string& inputString) {
        const int ADDITIONAL_SIZE = 3;
        std::vector<unsigned int> suffixArray(inputString.size() + ADDITIONAL_SIZE);

        std::vector<int> input;
        input.reserve(suffixArray.size());

        for (unsigned char c : inputString) {
            input.push_back(c);
        }

        // The Algorithm requires tha the last 3 elements are zeros
        for (int i = 0; i < ADDITIONAL_SIZE; ++i) {
            input.push_back(0);
        }

        buildSuffixArray(input.data(), (int*) suffixArray.data(), inputString.size(), 256);

        // Remove the 3 additional added zeros
        for (int i = 0; i < ADDITIONAL_SIZE; ++i) {
            suffixArray.pop_back();
        }

        return suffixArray;
    }

private:

    // lexicographic order for pairs
    static inline bool lexicographicCompare(int a1, int a2, int b1, int b2) {
        return (a1 < b1 || a1 == b1 && a2 <= b2);
    }

    // lexicographic order for triples
    static inline bool lexicographicCompare(int a1, int a2, int a3, int b1, int b2, int b3) {
        return (a1 < b1 || a1 == b1 && lexicographicCompare(a2, a3, b2, b3));
    }

    // Stably sort a[0..n-1] to b[0..n-1] with keys in 0..k from r
    static void radixSort(const int a[], int b[], const int r[], int n, int k) { // count occurrences
        std::vector<int> counterArray(k + 1, 0); // counter array

        // count occurrences
        for (int i = 0; i < n; i++) {
            counterArray[r[a[i]]]++;
        }

        // exclusive prefix sums
        for (int i = 0, sum = 0; i <= k; i++) {
            int temp = counterArray[i];
            counterArray[i] = sum;
            sum += temp;
        }

        // sort
        for (int i = 0; i < n; i++) {
            b[counterArray[r[a[i]]]++] = a[i];
        }
    }

    // Find the suffix array of T[0..n-1] in {1..K}^n
    // Require T[n]=T[n+1]=T[n+2]=0, n>=2
    static void buildSuffixArray(const int T[], int suffixArray[], int n, int K) {
        int n0 = (n + 2) / 3;
        int n1 = (n + 1) / 3;
        int n2 = n / 3;
        int n02 = n0 + n2;

        int* R = new int[n02 + 3];
        R[n02] = R[n02 + 1] = R[n02 + 2] = 0;
        int* SA12 = new int[n02 + 3];
        SA12[n02] = SA12[n02 + 1] = SA12[n02 + 2] = 0;
        int* R0 = new int[n0];
        int* SA0 = new int[n0];

        /*---------------------------------------- Step 0: Construct sample ----------------------------------------*/
        // Generate positions of mod 1 and mod 2 suffixes
        // "+(n0-n1)" adds a dummy mod 1 suffix if n%3 == 1
        for (int i = 0, j = 0; i < n + (n0 - n1); i++)
            if (i % 3 != 0)
                R[j++] = i;

        /*-------------------------------------- Step 1: Sort sample suffixes --------------------------------------*/
        // LSB radix sort the mod 1 and mod 2 triples
        radixSort(R, SA12, T + 2, n02, K);
        radixSort(SA12, R, T + 1, n02, K);
        radixSort(R, SA12, T, n02, K);
        // Find lexicographic names of triples and
        // Write them to correct places in R
        int name = 0, c0 = -1, c1 = -1, c2 = -1;
        for (int i = 0; i < n02; i++) {
            if (T[SA12[i]] != c0 || T[SA12[i] + 1] != c1 || T[SA12[i] + 2] != c2) {
                name++;
                c0 = T[SA12[i]];
                c1 = T[SA12[i] + 1];
                c2 = T[SA12[i] + 2];
            }
            if (SA12[i] % 3 == 1) {
                R[SA12[i] / 3] = name;
            } // write to R1
            else {
                R[SA12[i] / 3 + n0] = name;
            } // write to R2
        }
        // recurse if names are not yet unique
        if (name < n02) {
            buildSuffixArray(R, SA12, n02, name);
            // store unique names in R using the suffix array
            for (int i = 0; i < n02; i++)
                R[SA12[i]] = i + 1;
        } else // generate the suffix array of R directly
            for (int i = 0; i < n02; i++)
                SA12[R[i] - 1] = i;

        /*------------------------------------ Step 2: Sort nonsample suffixes ------------------------------------*/
        // stably sort the mod 0 suffixes from SA12 by their first character
        for (int i = 0, j = 0; i < n02; i++)
            if (SA12[i] < n0)
                R0[j++] = 3 * SA12[i];
        radixSort(R0, SA0, T, n0, K);

        /*---------------------------------------------- Step 3: Merge ---------------------------------------------*/
        // merge sorted SA0 suffixes and sorted SA12 suffixes
        for (int p = 0, t = n0 - n1, k = 0; k < n; k++) {
            #define GetI() (SA12[t] < n0 ? SA12[t] * 3 + 1 : (SA12[t] - n0) * 3 + 2)
            int i = GetI();    // pos of current offset 12 suffix
            int j = SA0[p];    // pos of current offset 0 suffix
            if (SA12[t] < n0 ? // different compares for mod 1 and mod 2 suffixes
                lexicographicCompare(T[i], R[SA12[t] + n0], T[j], R[j / 3])
                             : lexicographicCompare(T[i], T[i + 1], R[SA12[t] - n0 + 1], T[j], T[j + 1],
                                                    R[j / 3 + n0])) { // suffix from SA12 is smaller
                suffixArray[k] = i;
                t++;
                if (t == n02) // done --- only SA0 suffixes left
                    for (k++; p < n0; p++, k++)
                        suffixArray[k] = SA0[p];
            } else { // suffix from SA0 is smaller
                suffixArray[k] = j;
                p++;
                if (p == n0) // done --- only SA12 suffixes left
                    for (k++; t < n02; t++, k++)
                        suffixArray[k] = GetI();
            }
        }
        #undef GetI

        delete[] R;
        delete[] SA12;
        delete[] SA0;
        delete[] R0;
    }

};


#endif //SUFFIX_ARRAY_H
