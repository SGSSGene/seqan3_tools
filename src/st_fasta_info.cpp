#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/std/filesystem>
#include <sstream>

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_fasta_info", argc, argv};

    std::filesystem::path infile{};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";
    parser.add_positional_option(infile, "Please provide a fasta file.",
                                 seqan3::input_file_validator{{"fa", "fasta", "fna"}});

    std::string mode{"unknown"};
    parser.add_positional_option(mode, "Please provide a mode: names, lengths");


    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }


    seqan3::sequence_file_input fin{infile};
    if (mode == "lengths") {
        for (auto & record : fin) {
            std::cout << record.sequence().size() << "\n";
        }
    } else if (mode == "names") {
        for (auto & record : fin) {
            std::cout << record.id() << "\n";
        }
    } else {
        std::cout << "unknown mode: " << mode << "\n";
    }

    return EXIT_SUCCESS;
}
