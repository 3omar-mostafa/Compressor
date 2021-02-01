#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <queue>
#include <unordered_map>
#include <functional>
#include <cstdint>

#include "Node.h"
#include "../../Utils/BinaryIO.h"
#include <bit_string.h>


/**
 * David Huffman Encoding and Decoding
 *
 * @File_Format
 * ___________________________________________________   _ 
 * |        File_Header_Size_In_Bits (2 Bytes)       |    \
 * |_________________________________________________|     \ 
 * |            File Header (Dictionary):            |      \ 
 * |  Each Item consists of:                         |        Length of This Part in bits is File_Header_Size_In_Bits 
 * |   - Original Value (1 Byte) [ex. 'A' -> 0x41]   |        There may be some extra bits if size does not fit in bytes 
 * |   - Huffman_Code_Length (1 Byte)                |      / 
 * |   - Huffman Code (Huffman_Code_Length Bits)     |     / 
 * |                                                 |    / 
 * |_________________________________________________|   - 
 * |                                                 |
 * |      Huffman Coded Data (Rest of the file)      |
 * |_________________________________________________|
 * |        Extra Bits in Last Byte (1 Byte)         | 
 * |  There may be some extra bits, if size of Data  | 
 * |             does not fit in bytes               | 
 * --------------------------------------------------- 
 */
class Huffman {

    static const uint32_t BYTE = 8;

public:

    static void encode(const std::string& filename, const std::string& outputFileName) {

        std::string toBeEncoded = BinaryIO::readString(filename);

        auto frequencies = generateFrequencies(toBeEncoded);
        Node* huffmanTree = buildHuffmanTree(frequencies);
        auto huffmanCodes = generateHuffmanCodes(huffmanTree);

        deallocateHuffmanTree(huffmanTree); // release memory
        frequencies.clear(); // release memory

        bit_string binaryFileHeader = generateFileHeader(huffmanCodes);

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, binaryFileHeader);

        binaryFileHeader.clear();
        binaryFileHeader.shrink_to_fit(); // release memory

        bit_string encodedData = encode(toBeEncoded, huffmanCodes);

        BinaryIO::write(outputFileName, encodedData);
    }


    static void decode(const std::string& filename, const std::string& outputFileName) {

        // File Header Size In Bits is The First 2 Bytes of Data
        uint32_t fileHeaderSizeInBits = BinaryIO::readBitString(filename, 0, 2).to_uint_32();

        // The remaining of the file header is the huffman dictionary
        bit_string dictionary = BinaryIO::readBitString(filename, 2, byteSize(fileHeaderSizeInBits));

        uint32_t dictionarySizeInBits = fileHeaderSizeInBits - 2 * BYTE;

        // Remove Extra bits in last byte
        dictionary.pop_back(dictionary.size() - dictionarySizeInBits);

        auto huffmanCodes = reconstructHuffmanCodes(dictionary);
        dictionary.clear();
        dictionary.shrink_to_fit(); // release memory
        Node* huffmanTree = reconstructHuffmanTree(huffmanCodes);

        bit_string toBeDecoded = BinaryIO::readBitString(filename, byteSize(fileHeaderSizeInBits));

        bit_string decodedData = decode(toBeDecoded, huffmanCodes, huffmanTree);

        remove(outputFileName.c_str()); // Remove Output File If Exists
        BinaryIO::write(outputFileName, decodedData);

        deallocateHuffmanTree(huffmanTree); // release memory
    }


private:

    static uint32_t byteSize(uint32_t numberOfBits) {
        return numberOfBits / BYTE + (numberOfBits % BYTE != 0);
    }


    static bit_string encode(const std::string& toBeEncoded, std::unordered_map<uint8_t, bit_string>& huffmanCodes) {
        bit_string encodedData;
        encodedData.reserve(toBeEncoded.size() / (4 * BYTE));
        for (char c : toBeEncoded) {
            encodedData += huffmanCodes[c];
        }

        uint8_t extraBitsInLastByte = encodedData.extra_bits_size();
        for (int i = 0; i < extraBitsInLastByte; ++i) {
            encodedData.push_back(0);
        }

        encodedData += extraBitsInLastByte;
        return encodedData;
    }


    static bit_string decode(bit_string& toBeDecoded, std::unordered_map<bit_string, uint8_t>& huffmanCodes, Node* huffmanTree) {

        uint8_t extraBitsInLastByte = toBeDecoded.back_byte();
        toBeDecoded.pop_back(BYTE);
        toBeDecoded.pop_back(extraBitsInLastByte);

        bit_string decodedData;
        decodedData.reserve(toBeDecoded.size());

        bit_string currentCode;
        Node* currentNode = huffmanTree;
        for (bool toBeDecodedBit : toBeDecoded) {

            if (toBeDecodedBit == 1)
                currentNode = currentNode->left;
            else if (toBeDecodedBit == 0)
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


    static std::unordered_map<uint8_t, bit_string> generateHuffmanCodes(Node* huffmanTree) {
        std::unordered_map<uint8_t, bit_string> huffmanCodes;
        bit_string code;

        std::function<void(Node*)> generateHuffmanCodesRecursive = [&](Node* node) {
            if (node == nullptr)
                return;

            if (node->isLeaf()) {
                huffmanCodes.emplace(node->value, code);
                return;
            }

            code.push_back(1);
            generateHuffmanCodesRecursive(node->left);
            code.back() = 0;
            generateHuffmanCodesRecursive(node->right);
            code.pop_back();
        };

        generateHuffmanCodesRecursive(huffmanTree);

        return huffmanCodes;
    }


    // Read the File Header and construct the Huffman Codes Dictionary
    static std::unordered_map<bit_string, uint8_t> reconstructHuffmanCodes(const bit_string& dictionary) {

        std::unordered_map<bit_string, uint8_t> huffmanCodes(dictionary.size()/4);

        for (int i = 0; i < dictionary.size();) {
            uint8_t value = dictionary.substr(i, BYTE).to_uint_8();
            i += BYTE;
            uint32_t codeLength = dictionary.substr(i, BYTE).to_uint_32();
            i += BYTE;
            bit_string code = dictionary.substr(i, codeLength);
            i += codeLength;
            huffmanCodes[std::move(code)] = value;
        }

        return huffmanCodes;
    }


    static Node* buildHuffmanTree(const std::unordered_map<uint8_t, uint32_t>& frequencies) {
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


    static Node* reconstructHuffmanTree(const std::unordered_map<bit_string, uint8_t> &huffmanCodes) {

        Node* root = new Node;
        Node* currentNode;

        // for the readability purposes
        #define code first
        #define encodedValue second

        for (const auto& huffmanCode : huffmanCodes) {
            currentNode = root;
            for (bool currentCodeBit : huffmanCode.code) {
                if (currentCodeBit == 1) { // goes left
                    if (currentNode->left == nullptr) {
                        currentNode->left = new Node;
                    }
                    currentNode = currentNode->left;
                } else if (currentCodeBit == 0) { // goes right
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


    static std::unordered_map<uint8_t, uint32_t> generateFrequencies(const std::string& input) {
        std::unordered_map<uint8_t, uint32_t> frequencies(256);
        for (uint8_t currentChar : input) {
            frequencies[currentChar]++;
        }
        return frequencies;
    }


    static bit_string generateFileHeaderDictionary(const std::unordered_map<uint8_t , bit_string>& huffmanCodes) {
        #define encodedValue first
        #define code second

        bit_string dictionary;
        dictionary.reserve(huffmanCodes.size());

        for (auto& huffmanCode : huffmanCodes) {
            dictionary += huffmanCode.encodedValue;
            uint8_t currentCodeLength = huffmanCode.code.length();
            dictionary += currentCodeLength;
            dictionary += huffmanCode.code;
        }

        #undef code
        #undef encodedValue

        return dictionary;
    }


    static bit_string generateFileHeader(const std::unordered_map<uint8_t , bit_string>& huffmanCodes) {
        // TODO: More Optimization
        bit_string dictionary = generateFileHeaderDictionary(huffmanCodes);

        // The size of "fileHeaderSizeInBits" itself as it is added to the header
        uint16_t sizeOfFileHeaderSize = 2u * BYTE;
        uint16_t fileHeaderSizeInBits = sizeOfFileHeaderSize + dictionary.length();

        bit_string fileHeader = bit_string::from_uint_16(fileHeaderSizeInBits);
        fileHeader += dictionary;
        return fileHeader;
    }


};


#endif //HUFFMAN_H
