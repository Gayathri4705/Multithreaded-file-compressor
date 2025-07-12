#include "huffman.hpp"
#include <iostream>
#include <chrono>

namespace util {
    std::vector<uint8_t> readFile(const std::string&);   // implemented in compressor.cpp
    void writeFile(const std::string&, const std::vector<uint8_t>&);
    std::vector<uint8_t> multiCompress(const std::vector<uint8_t>&);
    std::vector<uint8_t> multiDecompress(const std::vector<uint8_t>&);
}

int main(int argc, char** argv) {
    if (argc!=4 || (std::string(argv[1])!="-c" && std::string(argv[1])!="-d")) {
        std::cerr << "Usage: "<<argv[0]<<" -c inputFile outputFile  (compress)\n"
                  << "       "<<argv[0]<<" -d inputFile outputFile  (decompress)\n";
        return 1;
    }
    bool compressMode = (std::string(argv[1])=="-c");
    auto inBuf  = util::readFile(argv[2]);

    auto t0 = std::chrono::high_resolution_clock::now();
    auto outBuf = compressMode ? util::multiCompress(inBuf)
                               : util::multiDecompress(inBuf);
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt = t1 - t0;

    util::writeFile(argv[3], outBuf);
    std::cout << (compressMode?"Compressed ":"Decompressed ")
              << inBuf.size() << " → " << outBuf.size()
              << " bytes in " << dt.count() << " s\n";
    return 0;
}
