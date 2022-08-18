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

    bool noMismatches{false};
    parser.add_flag(noMismatches, '\0', "noMismatches", "filter out all alignments with mismatches");

    bool noInsertions{false};
    parser.add_flag(noInsertions, '\0', "noInsertions", "filter out all alignments with insertions");

    bool noDeletions{false};
    parser.add_flag(noDeletions, '\0', "noDeletions", "filter out all alignments with Deletions");


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
        size_t countMismatches{};
        size_t countInsertions{};
        size_t countDeletions{};
        for (auto [ct, op] : record.cigar_sequence()) {
            if (op.to_char() != 'M' && op.to_char() != '=') {
                countErrors += ct;
            }
            if (op.to_char() == 'X') {
                countMismatches += ct;
            } else if (op.to_char() == 'I') {
                countInsertions += ct;
            } else if (op.to_char() == 'D') {
                countDeletions += ct;
            }
        }
        if (minErrors <= countErrors && countErrors <= error
            && (!noMismatches || countMismatches == 0)
            && (!noInsertions || countInsertions == 0)
            && (!noDeletions  || countDeletions == 0)) {

            fout.push_back(record);
        }
    }

    return EXIT_SUCCESS;
}
