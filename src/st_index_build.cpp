#include <seqan3/alphabet/adaptation/char.hpp>
#include <seqan3/alphabet/concept.hpp>
#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/search/fm_index/bi_fm_index.hpp>

struct my_dna4 : seqan3::sequence_file_input_default_traits_dna {
    using sequence_alphabet = seqan3::dna4;
};

struct my_dna5 : seqan3::sequence_file_input_default_traits_dna {
    using sequence_alphabet = seqan3::dna5;
};


template <typename trait>
void construct_index(std::filesystem::path infile, std::filesystem::path outfile, bool verbose) {
    using alphabet = trait::sequence_alphabet;

    if (verbose) seqan3::debug_stream << "Loading input file ... " << std::flush;
    auto fin  = seqan3::sequence_file_input<trait>{infile};
    auto sequences = std::vector<std::vector<alphabet>>{};
    for (auto & record : fin) {
        sequences.emplace_back(record.sequence());
    }
    if (verbose) seqan3::debug_stream << "done - loaded " << sequences.size() << " sequences\n";

    // creating the index
    if (verbose) seqan3::debug_stream << "Creating 2FM-Index ... " << std::flush;
    auto index = seqan3::bi_fm_index{sequences};
    if (verbose) seqan3::debug_stream << "done\n";

    // saving the index
    {
        if (verbose) seqan3::debug_stream << "Saving 2FM-Index ... " << std::flush;
        std::ofstream os{outfile, std::ios::binary};
        cereal::BinaryOutputArchive oarchive{os};
        oarchive(index);
        if (verbose) seqan3::debug_stream << "done\n";
    }

}

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_index_build", argc, argv};

    // Parser
    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path infile;
    std::filesystem::path outfile;
    bool verbose{false};

    parser.add_positional_option(infile, "Please provide a fasta file.",
                                 seqan3::input_file_validator{{"fa", "fasta", "fna"}});
    parser.add_positional_option(outfile, "output file, Please provide a storage path for the index");
    parser.add_flag(verbose, 'v', "verbose", "Activate verbose output.");

    bool use_dna4{false};
    parser.add_flag(use_dna4, '\0', "dna4", "Use dna 4 alphabet");


    try {
        parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }
    if (use_dna4) {
        construct_index<my_dna4>(infile, outfile, verbose);
    } else {
        construct_index<my_dna5>(infile, outfile, verbose);
    }


    return EXIT_SUCCESS;
}
