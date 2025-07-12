#include "bitio.hpp"

void BitWriter::pushBits(const std::string& code) {
    for(char bit : code) {
        cur_ = (cur_ << 1) | (bit == '1');
        if (++filled_ == 8) { buf_.push_back(cur_); cur_ = 0; filled_ = 0; }
    }
}
void BitWriter::flush() {
    if (filled_) { cur_ <<= (8 - filled_); buf_.push_back(cur_); filled_ = 0; }
}

BitReader::BitReader(const std::vector<uint8_t>& b) : buf_(b) {
    if (!buf_.empty()) cur_ = buf_[0];
}
int BitReader::nextBit() {
    if (idx_ >= buf_.size()) return -1;
    int bit = (cur_ >> 7) & 1;
    cur_ <<= 1;
    if (--left_ == 0) {
        idx_++;
        if (idx_ < buf_.size()) cur_ = buf_[idx_];
        left_ = 8;
    }
    return bit;
}
