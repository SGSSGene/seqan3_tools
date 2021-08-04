#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

struct Bitvector {
    struct Superblock {
        uint64_t superBlockEntry{};
        uint64_t blockEntries{};
        std::array<uint64_t, 6> bits{};

        uint64_t rank(size_t idx) const noexcept {
            assert(idx < 384);

            auto blockId = idx >> 6;
            auto block = 0b111111111ul & (blockEntries >> (blockId * 9));
            auto keep = (idx & 63);
            auto maskedBits = bits[blockId] << (63-keep);
            auto ct = std::bitset<64>{maskedBits}.count();

            auto total = superBlockEntry + block + ct;
            return total;
        }

        void setBlock(size_t blockId, size_t value) {
            blockEntries = blockEntries & ~uint64_t{0b111111111ul << blockId*9};
            blockEntries = blockEntries | uint64_t{value << blockId*9};
        }
    };


    std::vector<Superblock> superblocks{};

    uint64_t rank(size_t idx) const noexcept {
        auto superblockId = idx / 384;
        auto bitId        = idx % 384;
        return superblocks[superblockId].rank(bitId);
    }
};

