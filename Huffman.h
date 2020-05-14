#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <queue>
#include <map>

#include "Node.h"
#include "Converter.h"
#include "BinaryIO.h"

#define BYTE 8

#define ONE_MEGA_BYTE (1024*1024*8)

using namespace std;


class Huffman {

    map<char, string> encodingHuffmanCodes;
    map<string, char> decodingHuffmanCodes;
    map<char, int> frequencies;
    Node* huffmanTree = nullptr;
    const string fileSignature = "!huf_omar"; // to determine the  type of this file
    const string fileExtension = ".omar";

public:

    void compress(const string& filename) {

        string outputFileName = filename + fileExtension;
        remove(outputFileName.c_str()); // remove the file if exists

        string toBeCompressedString = BinaryIO::readBinaryFile(filename);

        generateFrequencies(toBeCompressedString);
        buildHuffmanTree();
        generateHuffmanCodes(huffmanTree);

        deallocateHuffmanTree(huffmanTree); // release memory
        frequencies.clear(); // release memory

        string binaryFileHeader = Converter::bitString_ToRealBinary(generateFileHeader());

        BinaryIO::writeBinaryFile(outputFileName, binaryFileHeader);

        binaryFileHeader.clear(); // release memory

        encode(toBeCompressedString, outputFileName);

        encodingHuffmanCodes.clear(); // release memory
    }


    virtual ~Huffman() {
        deallocateHuffmanTree(huffmanTree);
    }

private:


    unsigned int decodeFileHeaderSizeInBits(const string& compressedFileData) {

        int sizeOfFileHeaderSize = 2; // 2 bytes is the size of the (file header size)

        string headerFileSize = Converter::string_ToBitString(
                compressedFileData, fileSignature.length(), sizeOfFileHeaderSize);

        return Converter::bitString_ToInt(headerFileSize);
    }


    void generateHuffmanCodes(Node*& node, const string& code = "") {
        if (node == nullptr)
            return;

        // if leaf
        if (node->isLeaf()) {
            encodingHuffmanCodes[node->value] = code;
            return;
        }

        generateHuffmanCodes(node->left, code + "1");
        generateHuffmanCodes(node->right, code + "0");

    }


    void buildHuffmanTree() {
        priority_queue<Node*, vector<Node*>, Node::Compare> pq;

        // for the readability purposes
        #define key first
        #define freq second

        for (auto& frequency : frequencies) {
            pq.push(new Node(frequency.key, frequency.freq));
        }

        #undef key
        #undef freq

        while (pq.size() > 1) {
            Node* node1 = pq.top();
            pq.pop();
            Node* node2 = pq.top();
            pq.pop();

            pq.push((*node1) + node2);
        }

        huffmanTree = pq.top();
    }



    static void deallocateHuffmanTree(Node*& node) {
        if (node == nullptr)
            return;

        if (node->isLeaf()) {
            delete node;
            node = nullptr;
            return;
        }

        deallocateHuffmanTree(node->left);
        deallocateHuffmanTree(node->right);

        delete node;
        node = nullptr;
    }


    void generateFrequencies(const string& input) {
        for (char currentChar : input) {
            frequencies[currentChar]++;
        }
    }


    struct StringCompare {
        bool operator ()(const string& s1, const string& s2) {
            return s1.length() == s2.length() ? s1 < s2 : s1.length() < s2.length();
        }
    };


    map<string, char, StringCompare> getSortedHuffmanCodes() {

        map<string, char, StringCompare> sortedHuffmanCodes;

        #define encodedValue first
        #define code second

        // just swaps the map (key becomes value and value becomes key)
        for (auto& huffmanCode : encodingHuffmanCodes) {
            sortedHuffmanCodes[huffmanCode.code] = huffmanCode.encodedValue;
        }

        #undef encodedValue
        #undef code

        return sortedHuffmanCodes;
    }


    string generateFileHeaderDictionary() {
        #define code first
        #define encodedValue second

        string dictionary;

        map<string, char, StringCompare> sortedHuffmanCodes = getSortedHuffmanCodes();

        // the smallest code length is at sortedHuffmanCodes.begin() because it is sorted.
        int smallestCodeLength = sortedHuffmanCodes.begin()->code.length();

        // This is based on the fact that Huffman Codes are optimal
        // Therefore, when sorting them, the length difference between each two consecutive codes is 0 or 1
        // Therefore, we can store them in 1 bit only but we need the start to increment it which is smallestCodeLength
        dictionary += Converter::int8_ToBitString(smallestCodeLength);

        int currentCodeLength, previousCodeLength = smallestCodeLength;

        for (auto& huffmanCode : sortedHuffmanCodes) {
            dictionary += Converter::int8_ToBitString(huffmanCode.encodedValue);

            currentCodeLength = huffmanCode.code.length();
            dictionary += to_string(currentCodeLength - previousCodeLength); // The difference is 0 or 1 ONLY
            previousCodeLength = currentCodeLength;

            dictionary += huffmanCode.code;
        }

        #undef code
        #undef encodedValue

        return dictionary;
    }


    string generateFileHeader() {
        string fileHeader;

        fileHeader = Converter::string_ToBitString(fileSignature);

        string dictionary = generateFileHeaderDictionary();

        //  2 * BYTE is the size of "fileHeaderSize" itself as it is added to the header
        short fileHeaderSize = fileHeader.length() + 2 * BYTE + dictionary.length();

        // File Header Size in bits (it is 2 Bytes -> max of 8KB header)
        string fileHeaderSizeInBits = Converter::int16_ToBitString(fileHeaderSize);

        fileHeader += fileHeaderSizeInBits;
        fileHeader += dictionary;

        return fileHeader;
    }


    void encode(const string& toBeCompressedString, const string& outputFileName) {
        string encodedString, encodedData;

        for (char c : toBeCompressedString) {
            encodedString += encodingHuffmanCodes[c];

            // export to file and free exported memory every 1MB data
            if (encodedString.length() >= ONE_MEGA_BYTE) {
                int maxLengthDivisibleByBlockSize = encodedString.length() & (~(BYTE - 1));

                encodedData = Converter::bitString_ToRealBinary(encodedString, maxLengthDivisibleByBlockSize);
                encodedString.erase(0, maxLengthDivisibleByBlockSize);
                BinaryIO::writeBinaryFile(outputFileName, encodedData);
            }
        }

        encodedData = Converter::bitString_ToRealBinary(encodedString);
        BinaryIO::writeBinaryFile(outputFileName, encodedData);
        exportLastByteSize(encodedString, outputFileName);
    }


    static void exportLastByteSize(const string& encodedString, const string& outputFileName) {
        int maxLengthDivisibleByBlockSize = encodedString.length() & (~(BYTE - 1));

        int lastByteSize = encodedString.length() - maxLengthDivisibleByBlockSize;

        string binary = Converter::int8_ToBitString(lastByteSize);

        binary = Converter::bitString_ToRealBinary(binary);

        BinaryIO::writeBinaryFile(outputFileName, binary);
    }


};


#endif //HUFFMAN_H