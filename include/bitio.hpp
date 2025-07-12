#pragma once
#include <vector>
#include <cstdint>

class BitWriter {
    std::vector<uint8_t> buf_;
    uint8_t cur_{0}; int filled_{0};
public:
    void pushBits(const std::string& code);
    void flush();
    const std::vector<uint8_t>& data() const { return buf_; }
};

class BitReader {
    const std::vector<uint8_t>& buf_;
    size_t idx_{0}; int left_{8};
    uint8_t cur_{0};
public:
    explicit BitReader(const std::vector<uint8_t>& b);
    int nextBit();           // returns –1 when stream ends
};
