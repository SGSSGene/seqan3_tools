#include <seqan3/alphabet/adaptation/char.hpp>
#include <seqan3/alphabet/concept.hpp>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>

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
    seqan3::argument_parser parser{"st_text_map", argc, argv};

    // Parser
    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path infile;
    std::string mapping;
    std::filesystem::path outfile;

    parser.add_positional_option(infile, "Please provide a file.");
    parser.add_positional_option(outfile, "Please provide a output file.");

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
    writeFile(outfile, data);

    return EXIT_SUCCESS;
}
