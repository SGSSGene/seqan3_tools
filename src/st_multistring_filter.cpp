#include <filesystem>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <sstream>


struct Positions {
    std::vector<size_t> positions;
    Positions(std::filesystem::path infile) {
        auto posfile = std::filesystem::path{infile.string() + ".pos"};
        if (!exists(posfile)) {
            seqan3::sequence_file_input fin{infile};

            positions = {0};
            // iterate through all sequences in input file
            for (auto & record : fin) {
                auto seq = std::vector<seqan3::dna5>{record.sequence()};
                positions.push_back(positions.back() + seq.size());
            }

            auto ofs = std::ofstream(posfile.string());
            for (auto v : positions) {
                ofs << v << "\n";
            }
        } else {
            auto ifs = std::ifstream(posfile.string());
            size_t pos{};
            while(!ifs.eof()) {
                ifs >> pos;
                if (ifs.eof()) break;
                positions.push_back(pos);
            }
        }
    }

    auto translate(size_t pos, size_t len) -> std::optional<std::tuple<size_t, size_t>> {
        size_t idx{};
        while (pos >= positions[idx]) {
            ++idx;
        }
        if (pos + len > positions[idx]) return std::nullopt;
        pos -= positions[idx-1];
        return std::make_tuple(idx-1, pos);
    }
};

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_multistring_filter", argc, argv};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path infile{};
    parser.add_positional_option(infile, "Please provide a fasta file.");

    size_t length = 100;
    parser.add_positional_option(length, "length of the reads.");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }


    auto positions = Positions(infile);
    while(!std::cin.eof()) {
        size_t pos;
        std::cin >> pos;
        if (std::cin.eof()) break;
        auto r = positions.translate(pos, length);
        if (r) {
            auto [idx, pos] = *r;
            std::cout << idx << " " << pos << "\n";
        } else {
            std::cout << "_ _" << "\n";
        }
    }


    return EXIT_SUCCESS;
}
