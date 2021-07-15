#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/std/filesystem>
#include <sstream>

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_fasta_cut", argc, argv};

    std::filesystem::path infile{};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";
    parser.add_positional_option(infile, "Please provide a fasta file.",
                                 seqan3::input_file_validator{{"fa", "fasta", "fna"}});

    uint64_t maxBases = std::numeric_limits<uint64_t>::max();
    parser.add_option(maxBases, 'b', "max_bases", "max number of bases");

    uint64_t maxChr = std::numeric_limits<uint64_t>::max();
    parser.add_option(maxChr, 'c', "max_chr", "max number of chromosoms");



    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }


    seqan3::sequence_file_input fin{infile};
    seqan3::sequence_file_output fout{std::cout, seqan3::format_fasta{}};
    uint64_t ctBases{0};
    uint64_t ctChr{0};

    // iterate through all sequences in input file
    for (auto & record : fin) {
        if (ctChr == maxChr) break;
        if (ctBases == maxBases) break;

        auto seq = std::vector<seqan3::dna5>{record.sequence()};
        ctBases += seq.size();
        ctChr += 1;
        if (ctBases > maxBases) {
            seq.resize(seq.size() - (ctBases - maxBases));
            ctBases = maxBases;
        }
        fout.emplace_back(seq, record.id());
    }

    return EXIT_SUCCESS;
}
