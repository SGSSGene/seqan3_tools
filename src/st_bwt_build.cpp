#include <seqan3/alphabet/adaptation/char.hpp>
#include <seqan3/alphabet/concept.hpp>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>

#include <divsufsort64.h>

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


int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_index_build", argc, argv};

    // Parser
    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path infile;
    std::string mapping;
    std::filesystem::path outfile;

    parser.add_positional_option(infile, "Please provide a file.");
    parser.add_positional_option(outfile, "Please provdie a output file.");

    parser.add_option(mapping,  'm', "map", "Add a mapping e.g.: \"$ACGT\"");


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

    auto data = readFile(infile);
    if (!mapping.empty()) {
        for (auto& c : data) {
            c = map[c];
        }
    }

    auto sa = std::vector<int64_t>{};
    sa.resize(data.size());
    auto error = divsufsort64((uint8_t const*)data.data(), sa.data(), data.size());
    if (error != 0) {
        throw std::runtime_error("some error while creating the suffix array");
    }

    std::vector<uint8_t> bwt;
    bwt.resize(data.size());
    for (size_t i{0}; i < sa.size(); ++i) {
        bwt[i] = data[(sa[i] + data.size()- 1) % data.size()];
    }
    writeFile(outfile, bwt);


    return EXIT_SUCCESS;
}
