#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <queue>
#include <map>
#include <unordered_map>
#include <stdexcept>

#include "Node.h"
#include "../../Utils/Converter.h"
#include "../../Utils/BinaryIO.h"

#define BYTE 8

#define ONE_MEGA_BYTE (1024*1024*8)


class Huffman {

    std::map<char, std::string> encodingHuffmanCodes;
    std::map<std::string, char> decodingHuffmanCodes;
    std::map<char, int> frequencies;
    Node* huffmanTree = nullptr;
    const std::string fileSignature = "!huf_omar"; // to determine the  type of this file
    const std::string fileExtension = ".omar";

public:

    void compress(const std::string& filename) {

        std::string outputFileName = filename + fileExtension;
        remove(outputFileName.c_str()); // remove the file if exists

        std::string toBeCompressedString = BinaryIO::read(filename);

        generateFrequencies(toBeCompressedString);
        buildHuffmanTree();
        generateHuffmanCodes(huffmanTree);

        deallocateHuffmanTree(huffmanTree); // release memory
        frequencies.clear(); // release memory

        std::string binaryFileHeader = Converter::bitString_ToRealBinary(generateFileHeader());

        BinaryIO::write(outputFileName, binaryFileHeader);

        binaryFileHeader.clear(); // release memory

        encode(toBeCompressedString, outputFileName);

        encodingHuffmanCodes.clear(); // release memory
    }


    void decompress(const std::string& filename) {

        std::string compressedData = BinaryIO::read(filename);

        if (compressedData.substr(0, fileSignature.length()) != fileSignature) {
            throw std::domain_error("Wrong File Type");
        }

        reconstructHuffmanCodes(compressedData);
        reconstructHuffmanTree();

        int fileHeaderSizeInBits = decodeFileHeaderSizeInBits(compressedData);
        int fileHeaderSizeInBytes = fileHeaderSizeInBits / BYTE + (fileHeaderSizeInBits % BYTE != 0);

        compressedData.erase(0, fileHeaderSizeInBytes); // file header is no longer needed so we can release memory

        std::string outputFileName = filename + ".decoded";
        remove(outputFileName.c_str()); // remove the file if exists

        decode(compressedData, outputFileName);

        deallocateHuffmanTree(huffmanTree); // release memory
        decodingHuffmanCodes.clear(); // release memory

    }

    virtual ~Huffman() {
        deallocateHuffmanTree(huffmanTree);
    }

private:

    void decode(const std::string& toBeDecoded, const std::string& outputFileName) {

        std::string toBeDecodedBinaryString;
        std::string decodedData, currentCode;
        Node* currentNode = huffmanTree;

        int positionCompressedData = 0;
        do {

            toBeDecodedBinaryString = Converter::string_ToBitString(toBeDecoded, positionCompressedData,
                                                                    ONE_MEGA_BYTE);
            positionCompressedData += ONE_MEGA_BYTE;

            // if we reached end of the file
            if (positionCompressedData >= toBeDecoded.length()) {
                int sizeOfLastByte = toBeDecoded.back();
                // Remove the last 8 Bits from the string which were for the last byte size
                toBeDecodedBinaryString.erase(toBeDecodedBinaryString.length() - BYTE, BYTE);

                // Remove unneeded bits from last byte
                // example: if sizeOfLastByte is 5 and the last byte is 10100011 we remove 101
                toBeDecodedBinaryString.erase(toBeDecodedBinaryString.length() - BYTE,
                                              BYTE - sizeOfLastByte);
            }

            for (char toBeDecodedBit : toBeDecodedBinaryString) {

                if (toBeDecodedBit == '1')
                    currentNode = currentNode->left;
                else if (toBeDecodedBit == '0')
                    currentNode = currentNode->right;

                currentCode += toBeDecodedBit;

                if (currentNode->isLeaf()) {
                    decodedData += decodingHuffmanCodes[currentCode];
                    currentCode.clear();
                    currentNode = huffmanTree;
                }
            }

            // exports the decoded data until now to free some memory
            if (decodedData.length() >= ONE_MEGA_BYTE) {
                BinaryIO::write(outputFileName, decodedData);
                decodedData.clear();
            }

        } while (positionCompressedData < toBeDecoded.length());

        if (!decodedData.empty()) {
            BinaryIO::write(outputFileName, decodedData);
        }

    }


    unsigned int decodeFileHeaderSizeInBits(const std::string& compressedFileData) {

        int sizeOfFileHeaderSize = 2; // 2 bytes is the size of the (file header size)

        std::string headerFileSize = Converter::string_ToBitString(
                compressedFileData, fileSignature.length(), sizeOfFileHeaderSize);

        return Converter::bitString_ToInt(headerFileSize);
    }


    void generateHuffmanCodes(Node*& node, const std::string& code = "") {
        if (node == nullptr)
            return;

        if (node->isLeaf()) {
            encodingHuffmanCodes[node->value] = code;
            return;
        }

        generateHuffmanCodes(node->left, code + "1");
        generateHuffmanCodes(node->right, code + "0");

    }


    // Read the file Header and construct the Huffman Codes Dictionary
    void reconstructHuffmanCodes(const std::string& compressedFileData) {

        int fileHeaderSize = decodeFileHeaderSizeInBits(compressedFileData);

        // 2 -> size of (file header size)
        int smallestCodeLengthIndex = fileSignature.length() + 2;
        int smallestCodeLength = compressedFileData[smallestCodeLengthIndex];

        int dictionaryStartIndex = smallestCodeLengthIndex + 1;
        int dictionaryLengthInBits = fileHeaderSize - (dictionaryStartIndex * BYTE);

        std::string dictionaryBits = Converter::string_ToBitString(
                compressedFileData, dictionaryStartIndex, dictionaryLengthInBits / BYTE + 1);

        // if the header does not fit in bytes, i.e. there are extra bits which does not belong to it
        if (dictionaryLengthInBits % BYTE != 0) {
            int bitsToEraseLength = dictionaryBits.length() - dictionaryLengthInBits;
            dictionaryBits.erase(dictionaryBits.length() - BYTE, bitsToEraseLength);
        }

        int codeLength = smallestCodeLength;
        char value;
        std::string code;
        for (int i = 0; i < dictionaryLengthInBits;) {
            value = Converter::bitString_ToRealBinary(dictionaryBits, i, BYTE)[0];
            i += BYTE;
            codeLength += dictionaryBits[i] == '1' ? 1 : 0; // to understand this look at dictionary encoding
            i++;
            code = dictionaryBits.substr(i, codeLength);
            i += codeLength;
            decodingHuffmanCodes[code] = value;
        }

    }


    void buildHuffmanTree() {
        std::priority_queue<Node*, std::vector<Node*>, Node::Compare> pq;

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


    void reconstructHuffmanTree() {

        Node* root = new Node;
        Node* currentNode;

        // for the readability purposes
        #define code first
        #define encodedValue second

        for (auto& huffmanCode : decodingHuffmanCodes) {
            currentNode = root;
            for (char currentCodeBit : huffmanCode.code) {
                if (currentCodeBit == '1') { // goes left
                    if (currentNode->left == nullptr) {
                        currentNode->left = new Node;
                    }
                    currentNode = currentNode->left;
                } else if (currentCodeBit == '0') { // goes right
                    if (currentNode->right == nullptr) {
                        currentNode->right = new Node;
                    }
                    currentNode = currentNode->right;
                }
            }

            currentNode->value = huffmanCode.encodedValue;
        }

        // un define them in order not to mess with other stuff outside this function
        #undef code
        #undef encodedValue

        huffmanTree = root;
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


    void generateFrequencies(const std::string& input) {
        for (char currentChar : input) {
            frequencies[currentChar]++;
        }
    }


    struct StringCompare {
        bool operator ()(const std::string& s1, const std::string& s2) {
            return s1.length() == s2.length() ? s1 < s2 : s1.length() < s2.length();
        }
    };


    std::map<std::string, char, StringCompare> getSortedHuffmanCodes() {

        std::map<std::string, char, StringCompare> sortedHuffmanCodes;

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


    std::string generateFileHeaderDictionary() {
        #define code first
        #define encodedValue second

        std::string dictionary;

        std::map<std::string, char, StringCompare> sortedHuffmanCodes = getSortedHuffmanCodes();

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
            dictionary += std::to_string(currentCodeLength - previousCodeLength); // The difference is 0 or 1 ONLY
            previousCodeLength = currentCodeLength;

            dictionary += huffmanCode.code;
        }

        #undef code
        #undef encodedValue

        return dictionary;
    }


    std::string generateFileHeader() {
        std::string fileHeader;

        fileHeader = Converter::string_ToBitString(fileSignature);

        std::string dictionary = generateFileHeaderDictionary();

        //  2 * BYTE is the size of "fileHeaderSize" itself as it is added to the header
        short fileHeaderSize = fileHeader.length() + 2 * BYTE + dictionary.length();

        // File Header Size in bits (it is 2 Bytes -> max of 8KB header)
        std::string fileHeaderSizeInBits = Converter::int16_ToBitString(fileHeaderSize);

        fileHeader += fileHeaderSizeInBits;
        fileHeader += dictionary;

        return fileHeader;
    }


    void encode(const std::string& toBeCompressedString, const std::string& outputFileName) {
        std::string encodedString, encodedData;

        for (char c : toBeCompressedString) {
            encodedString += encodingHuffmanCodes[c];

            // export to file and free exported memory every 1MB data
            if (encodedString.length() >= ONE_MEGA_BYTE) {
                int maxLengthDivisibleByBlockSize = encodedString.length() & (~(BYTE - 1));

                encodedData = Converter::bitString_ToRealBinary(encodedString, maxLengthDivisibleByBlockSize);
                encodedString.erase(0, maxLengthDivisibleByBlockSize);
                BinaryIO::write(outputFileName, encodedData);
            }
        }

        encodedData = Converter::bitString_ToRealBinary(encodedString);
        BinaryIO::write(outputFileName, encodedData);
        exportLastByteSize(encodedString, outputFileName);
    }


    static void exportLastByteSize(const std::string& encodedString, const std::string& outputFileName) {
        int maxLengthDivisibleByBlockSize = encodedString.length() & (~(BYTE - 1));

        int lastByteSize = encodedString.length() - maxLengthDivisibleByBlockSize;

        std::string binary = Converter::int8_ToBitString(lastByteSize);

        binary = Converter::bitString_ToRealBinary(binary);

        BinaryIO::write(outputFileName, binary);
    }


};


#endif //HUFFMAN_H