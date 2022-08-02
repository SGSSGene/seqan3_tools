#include <filesystem>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
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

    uint64_t skipChr{};
    parser.add_option(skipChr, 's', "skip_chr", "skip the first chr");

    uint64_t maxBasesPerLine = std::numeric_limits<uint64_t>::max();
    parser.add_option(maxBasesPerLine, '\0', "max_bases_per_line", "resizes lines to maximum bases");

    uint64_t minBasesPerLine = 0;
    parser.add_option(minBasesPerLine, '\0', "min_bases_per_line", "skips lines with less entries");


    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }


    seqan3::sequence_file_input fin{infile};
    seqan3::sequence_file_output fout{std::cout, seqan3::format_fasta{}};
    fout.options.fasta_blank_before_id = false;
    uint64_t ctBases{0};
    uint64_t ctChr{0};

    // iterate through all sequences in input file
    for (auto & record : fin) {
        if (skipChr > 0) {
            skipChr -= 1;
            continue;
        }
        if (ctChr == maxChr) break;
        if (ctBases == maxBases) break;

        auto seq = std::vector<seqan3::dna5>{record.sequence()};
        if (seq.size() < minBasesPerLine) continue;

        ctBases += seq.size();
        ctChr += 1;
        if (ctBases > maxBases) {
            seq.resize(seq.size() - (ctBases - maxBases));
            ctBases = maxBases;
        }
        if (seq.size() > maxBasesPerLine) {
            ctBases -= (seq.size() - maxBasesPerLine);
            seq.resize(maxBasesPerLine);
        }
        fout.emplace_back(seq, record.id());
    }

    return EXIT_SUCCESS;
}
