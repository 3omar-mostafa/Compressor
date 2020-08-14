#ifndef NODE_H
#define NODE_H

#include <cstdint>

class Node {

public:

    uint8_t value;
    uint32_t freq;
    Node* left;
    Node* right;

    Node() {
        value = freq =0;
        left = right = nullptr;
    }

    Node(uint8_t value, uint32_t freq) : value(value), freq(freq) {
        left = right = nullptr;
    }

    bool operator <(const Node& rhs) const {
        // inverted the sign in order to have min heap in priority_queue
        return freq > rhs.freq;
    }

    bool isLeaf() const {
        return this->left == nullptr && this->right == nullptr;
    }

    Node* operator +(Node* rhs) {
        Node* node = new Node;
        node->right = rhs;
        node->left = this;
        node->freq = this->freq + rhs->freq;
        return node;
    }


    // Compare Node pointers
    struct Compare {
        bool operator ()(Node* node1, Node* node2) {
            return *node1 < *node2;
        }
    };

};


#endif //NODE_H
