#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <queue>
#include <unordered_map>
#include <functional>
#include <cstdint>

#include "Node.h"
#include "../../Utils/Converter.h"
#include "../../Utils/BinaryIO.h"

#define BYTE 8u

class Huffman {

public:

    static void encode(const std::string& filename, const std::string& outputFileName) {

        std::string toBeEncoded = BinaryIO::read(filename);

        auto frequencies = generateFrequencies(toBeEncoded);
        Node* huffmanTree = buildHuffmanTree(frequencies);
        auto huffmanCodes = generateHuffmanCodes(huffmanTree);

        deallocateHuffmanTree(huffmanTree); // release memory
        frequencies.clear(); // release memory

        std::string binaryFileHeader = Converter::bitString_ToRealBinary(generateFileHeader(huffmanCodes));

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, binaryFileHeader);

        binaryFileHeader.clear();
        binaryFileHeader.shrink_to_fit(); // release memory

        std::string encodedData = encode(toBeEncoded, huffmanCodes);

        BinaryIO::write(outputFileName, encodedData);
    }


    static void decode(const std::string& filename, const std::string& outputFileName) {

        uint32_t fileHeaderSizeInBytes = byteSize(decodeFileHeaderSizeInBits(BinaryIO::read(filename, 0, 2)));

        std::string fileHeader = BinaryIO::read(filename, 0, fileHeaderSizeInBytes);

        auto huffmanCodes = reconstructHuffmanCodes(fileHeader);
        Node* huffmanTree = reconstructHuffmanTree(huffmanCodes);

        std::string toBeDecoded = BinaryIO::read(filename, fileHeaderSizeInBytes);

        std::string decodedData = decode(toBeDecoded, huffmanCodes, huffmanTree);

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, decodedData);

        deallocateHuffmanTree(huffmanTree); // release memory
    }


private:

    static uint32_t byteSize(uint32_t numberOfBits) {
        return numberOfBits / BYTE + (numberOfBits % BYTE != 0);
    }


    static std::string encode(const std::string& toBeEncoded, std::unordered_map<char, std::string>& huffmanCodes) {
        std::string encodedString;
        for (char c : toBeEncoded) {
            encodedString += huffmanCodes[c];
        }

        std::string encodedData = Converter::bitString_ToRealBinary(encodedString);
        uint8_t extraBitsInLastByte = getNumberOfExtraBitsInLastByte(encodedString);

        encodedData += extraBitsInLastByte;

        return encodedData;
    }


    static std::string decode(std::string& toBeDecoded, std::unordered_map<std::string, char>& huffmanCodes, Node* huffmanTree) {

        int extraBitsInLastByte = toBeDecoded.back();
        toBeDecoded.pop_back();

        std::string toBeDecodedBitString = Converter::string_ToBitString(toBeDecoded);

        toBeDecodedBitString.erase(toBeDecodedBitString.length() - extraBitsInLastByte);

        std::string decodedData, currentCode;
        Node* currentNode = huffmanTree;
        for (char toBeDecodedBit : toBeDecodedBitString) {

            if (toBeDecodedBit == '1')
                currentNode = currentNode->left;
            else if (toBeDecodedBit == '0')
                currentNode = currentNode->right;

            currentCode += toBeDecodedBit;

            if (currentNode->isLeaf()) {
                decodedData += huffmanCodes[currentCode];
                currentCode.clear();
                currentNode = huffmanTree;
            }
        }
        return decodedData;
    }

    static uint32_t decodeFileHeaderSizeInBits(const std::string& fileHeader) {
        uint32_t fileHeaderSizeInBits = (uint8_t) fileHeader[0];
        fileHeaderSizeInBits <<= BYTE;
        fileHeaderSizeInBits += (uint8_t) fileHeader[1];
        return fileHeaderSizeInBits;
    }


    static std::unordered_map<char , std::string> generateHuffmanCodes(Node* huffmanTree) {
        std::unordered_map<char, std::string> huffmanCodes;
        std::string code;

        std::function<void(Node*)> generateHuffmanCodesRecursive = [&](Node* node) {
            if (node == nullptr)
                return;

            if (node->isLeaf()) {
                huffmanCodes.emplace(node->value, code);
                return;
            }

            code.push_back('1');
            generateHuffmanCodesRecursive(node->left);
            code.back() = '0';
            generateHuffmanCodesRecursive(node->right);
            code.pop_back();
        };

        generateHuffmanCodesRecursive(huffmanTree);

        return huffmanCodes;
    }


    // Read the file Header and construct the Huffman Codes Dictionary
    static std::unordered_map<std::string, char> reconstructHuffmanCodes(const std::string& fileHeader) {

        uint32_t dictionaryLengthInBits = decodeFileHeaderSizeInBits(fileHeader) - (2u * BYTE);

        std::string dictionaryBits = Converter::string_ToBitString(fileHeader, 2, fileHeader.length());

        // if the header does not fit in bytes, i.e. there are extra bits which does not belong to it
        while (dictionaryBits.length() > dictionaryLengthInBits)
            dictionaryBits.pop_back();

        std::unordered_map<std::string, char> huffmanCodes;
        uint8_t value;
        std::string code;
        for (int i = 0; i < dictionaryLengthInBits;) {
            value = Converter::bitString_ToRealBinary(dictionaryBits, i, BYTE)[0];
            i += BYTE;
            uint32_t codeLength = Converter::bitString_ToInt(dictionaryBits.substr(i, BYTE));
            i += BYTE;
            code = dictionaryBits.substr(i, codeLength);
            i += codeLength;
            huffmanCodes[code] = value;
        }

        return huffmanCodes;
    }


    static Node* buildHuffmanTree(const std::unordered_map<char, int>& frequencies) {
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

        return pq.top();
    }


    static Node* reconstructHuffmanTree(const std::unordered_map<std::string, char> &huffmanCodes) {

        Node* root = new Node;
        Node* currentNode;

        // for the readability purposes
        #define code first
        #define encodedValue second

        for (const auto& huffmanCode : huffmanCodes) {
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

        return root;
    }


    static void deallocateHuffmanTree(Node* node) {
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


    static std::unordered_map<char, int> generateFrequencies(const std::string& input) {
        std::unordered_map<char, int> frequencies;
        for (char currentChar : input) {
            frequencies[currentChar]++;
        }
        return frequencies;
    }


    static std::string generateFileHeaderDictionary(const std::unordered_map<char, std::string>& huffmanCodes) {
        #define encodedValue first
        #define code second

        std::string dictionary;

        for (auto& huffmanCode : huffmanCodes) {
            dictionary += Converter::int8_ToBitString(huffmanCode.encodedValue);
            uint8_t currentCodeLength = huffmanCode.code.length();
            dictionary += Converter::int8_ToBitString(currentCodeLength);
            dictionary += huffmanCode.code;
        }

        #undef code
        #undef encodedValue

        return dictionary;
    }


    static std::string generateFileHeader(const std::unordered_map<char, std::string>& huffmanCodes) {

        std::string dictionary = generateFileHeaderDictionary(huffmanCodes);

        //  2u * BYTE is the size of "fileHeaderSize" itself as it is added to the header
        uint16_t fileHeaderSize = 2u * BYTE + dictionary.length();

        // File Header Size in bits (it is 2 Bytes -> max of 8KB header)
        std::string fileHeaderSizeInBits = Converter::int16_ToBitString(fileHeaderSize);

        std::string fileHeader = fileHeaderSizeInBits + dictionary;
        return fileHeader;
    }

    static uint8_t getNumberOfExtraBitsInLastByte(const std::string& encodedString) {
        int maxLengthFitInBytes = encodedString.length() & (~(BYTE - 1u));
        return (BYTE - (encodedString.length() - maxLengthFitInBytes));
    }


};


#endif //HUFFMAN_H