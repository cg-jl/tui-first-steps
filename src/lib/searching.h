#pragma once

template <size_t errorMargin = 3>
bool fuzzyFind(const std::string& needle, const std::string& haystack) {
    size_t m = needle.size();
    if (m == 0) return true;
    if (m > 63)
        return false;  // shouldn't be the case of m > bits(std::uint64_t)

    std::uint64_t pattern_mask[128];
    std::uint64_t R[errorMargin + 1];
    size_t i;
    // init masks

    for (auto& r : R) r = ~1;
    for (auto& m : pattern_mask) m = ~0;
    for (i = 0; i < m; ++i)
        pattern_mask[(unsigned char)needle[i]] &= ~(1ULL << i);

    for (i = 0; i < haystack.size(); ++i) {
        unsigned char c = (unsigned char)haystack[i];
        std::uint64_t oldRd1 = R[0];

        R[0] |= pattern_mask[c];
        R[0] <<= 1;

        for (auto& r : R) {
            auto tmp = r;
            r = (oldRd1 & (r | pattern_mask[c])) << 1;
            oldRd1 = tmp;
        }

        if ((R[errorMargin] & (1UL << m)) == 0) return true;
    }
    return false;
}