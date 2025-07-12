#include "huffman.hpp"
#include "bitio.hpp"
#include <queue>
#include <stdexcept>

Node::Node(uint8_t c, uint64_t f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
Node::Node(Node* l, Node* r) : ch(0), freq(l->freq + r->freq), left(l), right(r) {}
bool NodeCmp::operator()(Node* a, Node* b) { return a->freq > b->freq; }

Node* Huffman::buildTree(const std::array<uint64_t,256>& freq) {
    std::priority_queue<Node*, std::vector<Node*>, NodeCmp> pq;
    for (int i=0;i<256;++i) if (freq[i]) pq.push(new Node(i,freq[i]));
    if (pq.size()==1) pq.push(new Node(0,1));          // handle degenerate case
    while (pq.size()>1) {
        Node* a=pq.top(); pq.pop();
        Node* b=pq.top(); pq.pop();
        pq.push(new Node(a,b));
    }
    return pq.top();
}

void Huffman::genCodes(Node* root, const std::string& cur,
                       std::array<std::string,256>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->ch] = cur.empty() ? "0" : cur;
        return;
    }
    genCodes(root->left ,cur+"0",codes);
    genCodes(root->right,cur+"1",codes);
}

/* ---------------- compress ---------------- */
std::vector<uint8_t> Huffman::compress(const std::vector<uint8_t>& in) {
    std::array<uint64_t,256> freq{}; for (auto b:in) ++freq[b];
    Node* root = buildTree(freq);
    std::array<std::string,256> codes;
    genCodes(root,"",codes);

    BitWriter bw; for (auto b:in) bw.pushBits(codes[b]); bw.flush();

    /* Build container */
    std::vector<uint8_t> out;
    auto emitLE=[&](uint64_t v,int n){ for(int i=0;i<n;++i) out.push_back(uint8_t(v>>(i*8))); };

    out.insert(out.end(),{'H','U','F',0});                    // magic
    emitLE(uint32_t(in.size()),4);                           // original size
    emitLE(uint16_t(256*8),2);                               // table size (fixed)
    for (auto f:freq) emitLE(f,8);                           // frequencies
    const auto& bits=bw.data();
    out.insert(out.end(),bits.begin(),bits.end());
    return out;
}

/* ---------------- decompress ---------------- */
std::vector<uint8_t> Huffman::decompress(const std::vector<uint8_t>& in) {
    size_t p=0; auto rdLE=[&](int n){ uint64_t v=0; for(int i=0;i<n;++i)v|=uint64_t(in[p++])<<(i*8); return v; };
    if(!(in[p]=='H'&&in[p+1]=='U'&&in[p+2]=='F'&&in[p+3]==0)) throw std::runtime_error("bad header"); p+=4;
    uint32_t origSz  = rdLE(4);  rdLE(2);                    // skip table size
    std::array<uint64_t,256> freq{}; for(int i=0;i<256;++i) freq[i]=rdLE(8);

    Node* root=buildTree(freq);
    std::vector<uint8_t> bits(in.begin()+p,in.end());
    BitReader br(bits); std::vector<uint8_t> out; out.reserve(origSz);
    Node* cur=root;
    while(out.size()<origSz){
        int bit=br.nextBit(); if(bit==-1) break;
        cur = bit ? cur->right : cur->left;
        if(!cur->left && !cur->right){ out.push_back(cur->ch); cur=root; }
    }
    return out;
}
