#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

struct Node {
    uint8_t ch;
    uint64_t freq;
    Node *left, *right;
    Node(uint8_t c, uint64_t f);
    Node(Node* l, Node* r);
};
struct NodeCmp {
    bool operator()(Node* a, Node* b);
};

class Huffman {
public:
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& in);
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& in);

private:
    static Node* buildTree(const std::array<uint64_t,256>& freq);
    static void  genCodes(Node* root, const std::string& cur,
                          std::array<std::string,256>& codes);
};
