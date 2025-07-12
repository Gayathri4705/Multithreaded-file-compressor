#include "huffman.hpp"
#include <fstream>
#include <thread>

namespace util {
std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("open " + path);
    return { std::istreambuf_iterator<char>(f), {} };
}
void writeFile(const std::string& path, const std::vector<uint8_t>& buf) {
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(buf.data()), buf.size());
}
} // namespace util

/* --------- multi‑threaded wrapper --------- */
std::vector<uint8_t> multiCompress(const std::vector<uint8_t>& buf) {
    size_t nT = std::thread::hardware_concurrency(); if(!nT) nT=4;
    size_t chunk = (buf.size() + nT - 1) / nT;

    std::vector<std::vector<uint8_t>> parts(nT);
    std::vector<std::thread> workers;
    for (size_t t=0;t<nT;++t) {
        size_t l=t*chunk, r=std::min(buf.size(), l+chunk);
        if(l>=r) continue;
        workers.emplace_back([&,t,l,r]{ parts[t]=Huffman::compress({buf.begin()+l,buf.begin()+r}); });
    }
    for (auto& th:workers) th.join();

    std::vector<uint8_t> out;
    out.insert(out.end(), {'P', static_cast<uint8_t>(nT)});     // header: multi-part
    for (auto& p:parts) { uint32_t len=p.size(); out.push_back(len&0xFF); out.push_back(len>>8); }
    for (auto& p:parts)   out.insert(out.end(), p.begin(), p.end());
    return out;
}

std::vector<uint8_t> multiDecompress(const std::vector<uint8_t>& buf) {
    if (buf[0]!='P') return Huffman::decompress(buf);          // single part
    size_t parts = buf[1], p = 2;
    std::vector<uint32_t> len(parts);
    for (size_t i=0;i<parts;++i){ len[i]=buf[p] | (buf[p+1]<<8); p+=2; }

    std::vector<uint8_t> out;
    for (size_t i=0;i<parts;++i) {
        std::vector<uint8_t> sub(buf.begin()+p, buf.begin()+p+len[i]); p+=len[i];
        auto dec = Huffman::decompress(sub);
        out.insert(out.end(), dec.begin(), dec.end());
    }
    return out;
}
