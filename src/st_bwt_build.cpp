#include "utils/Bitvector.h"

#include <seqan3/alphabet/adaptation/char.hpp>
#include <seqan3/alphabet/concept.hpp>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>

#include <divsufsort64.h>

struct StopWatch {
    using TP = decltype(std::chrono::steady_clock::now());

    TP start{std::chrono::steady_clock::now()};

    auto now() const -> TP {
        return std::chrono::steady_clock::now();
    }

    auto reset() -> double {
        auto t = now();
        auto diff = t - start;
        start = t;
        return diff.count() / 1'000'000'000.;
    }

    auto peek() const -> double {
        auto t = now();
        auto diff = t - start;
        return diff.count() / 1'000'000'000.;
    }
};



auto readFile(std::filesystem::path const& file) -> std::vector<uint8_t> {
    auto ifs = std::ifstream{file, std::ios::binary};
    ifs.seekg(0, std::ios::end);
    std::size_t size = ifs.tellg();
    auto buffer = std::vector<uint8_t>(size);
    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    return buffer;
}

void writeFile(std::filesystem::path const& file, std::vector<uint8_t> const& buffer) {
    auto ofs = std::ofstream{file, std::ios::binary};
    ofs.write(reinterpret_cast<char const*>(buffer.data()), buffer.size());
}

template <typename T>
void writeFile(std::filesystem::path const& file, std::vector<T> const& buffer) {
    std::vector<uint8_t> buffer2;
    buffer2.resize(buffer.size() * sizeof(T));
    memcpy(buffer2.data(), buffer.data(), buffer2.size());
    writeFile(file, buffer);
}
struct BitStack {
    uint64_t size{0};
    uint64_t ones{0};
    std::vector<uint8_t> data;
    void push(bool bit) {
        if (size % 8 == 0) {
            data.push_back(0);
        }
        data.back() = data.back() | (bit << (size % 8));
        size += 1;
        if (bit) {
            ones += 1;
        }
    }

    bool value(size_t idx) const {
        auto block  = idx / 8;
        auto bitNbr = idx % 8;
        return (data[block] & (1ul << bitNbr));
    }

    void append(std::vector<uint8_t>& buffer) const {
        buffer.reserve(buffer.size() + 16 + data.size());
        {
            buffer.resize(buffer.size() + 8);
            memcpy(buffer.data() + buffer.size() - 8, &size, sizeof(size));
        }
        {
            buffer.resize(buffer.size() + 8);
            memcpy(buffer.data() + buffer.size() - 8, &ones, sizeof(ones));
        }

        for (auto e : data) {
            buffer.push_back(e);
        }
        if (data.size() % 8 > 0) {
            for (size_t i{data.size() % 8}; i < 8; ++i) {
                buffer.push_back(0);
            }
        }
    }


    size_t read(uint8_t const* buffer, size_t len) {
        assert(len >= 16);

        memcpy(&size, buffer, 8); buffer += 8;
        memcpy(&ones, buffer, 8); buffer += 8;

        data.resize(size / 8 + (((size % 8) > 0)?1:0));
        memcpy(data.data(), buffer, data.size());
        if (data.size() % 8 > 0) {
            return 16 + data.size() + (8-(data.size() % 8));
        }
        return 16 + data.size();
    }
};


int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_bwt_build", argc, argv};

    // Parser
    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path infile;
    std::string mapping;
    std::filesystem::path outfile;

    parser.add_positional_option(infile, "Please provide a file.");
    parser.add_positional_option(outfile, "Please provdie a output file.");

    parser.add_option(mapping,  'm', "map", "Add a mapping e.g.: \"$ACGT\"");

    std::filesystem::path csaFile;
    uint64_t csaRate{1};

    parser.add_option(csaFile, 'a', "csa_file", "suffix array file");
    parser.add_option(csaRate, 'b', "csa_rate", "compression rate");


    try {
        parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    auto map = std::array<uint8_t, 256>{0};
    for (size_t i{0}; i < mapping.size(); ++i) {
        map[mapping[i]] = i;
    }

    StopWatch stopWatch;

    auto data = readFile(infile);

    auto time_read = stopWatch.reset();
    std::cout << "reading file took " << time_read << "s\n";

    if (!mapping.empty()) {
        for (auto& c : data) {
            c = map[c];
        }
    }

    auto time_map = stopWatch.reset();
    std::cout << "mapping/counting took " << time_map << "s\n";


    auto sa = std::vector<int64_t>{};
    sa.resize(data.size());
    auto error = divsufsort64((uint8_t const*)data.data(), sa.data(), data.size());
    if (error != 0) {
        throw std::runtime_error("some error while creating the suffix array");
    }

    auto time_sa = stopWatch.reset();
    std::cout << "sa construction took " << time_sa << "s\n";


    std::vector<uint8_t> bwt;
    bwt.resize(data.size());
    for (size_t i{0}; i < sa.size(); ++i) {
        bwt[i] = data[(sa[i] + data.size()- 1) % data.size()];
    }

    auto time_bwt = stopWatch.reset();
    std::cout << "bwt took " << time_bwt << "s\n";

    writeFile(outfile, bwt);

    auto time_write = stopWatch.reset();
    std::cout << "writing to file took " << time_write << "s\n";


    if (csaFile != "") {
        BitStack bits;
        std::vector<uint64_t> csa;
        for (auto e : sa) {
            if (e % csaRate == 0) {
                bits.push(1);
                csa.push_back(e);
            } else {
                bits.push(0);
            }
        }
        std::vector<uint8_t> buffer;
        bits.append(buffer);
        auto len = buffer.size();
        buffer.resize(buffer.size() + csa.size() * 8);
        memcpy(buffer.data() + len, csa.data(), csa.size() * 8);
        writeFile(csaFile, buffer);
    }
    auto time_csa = stopWatch.reset();
    std::cout << "csa construction took " << time_csa << "s\n";

    return EXIT_SUCCESS;
}
