#include <filesystem>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/io/sam_file/all.hpp>
#include <sstream>


int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_sam_filter", argc, argv};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";


    std::filesystem::path in_file{};
    parser.add_positional_option(in_file, "input");

    std::filesystem::path out_file{};
    parser.add_positional_option(out_file, "output");

    size_t error = 0;
    parser.add_option(error, '\0', "error", "max number of allowed errors");

    size_t minErrors = 0;
    parser.add_option(minErrors, '\0', "minError", "min number of occured errors");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }


    auto fin  = seqan3::sam_file_input{in_file};
    auto fout = seqan3::sam_file_output{out_file};

    for (auto & record : fin) {
        size_t countErrors{};
        for (auto [ct, op] : record.cigar_sequence()) {
            if (op.to_char() != 'M' && op.to_char() != '=') {
                countErrors += 1;
            }
        }
        if (minErrors <= countErrors && countErrors <= error) {
            fout.push_back(record);
        }
    }

    return EXIT_SUCCESS;
}
