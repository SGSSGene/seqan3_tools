// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <ctime>
#include <filesystem>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <sstream>

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_dna5todna4", argc, argv};

    std::filesystem::path infile{};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";
    parser.add_positional_option(infile, "Please provide a fasta file.",
                                 seqan3::input_file_validator{{"fa", "fasta", "fna"}});

    int seed{-1};
    parser.add_option(seed, '\0', "seed", "seed to generate data, if value -1 current time is being used.");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    // conversion from dna5 to dna4
    seqan3::sequence_file_input fin{infile};
    seqan3::sequence_file_output fout{std::cout, seqan3::format_fasta{}};

    if (seed == -1) {
        seed = time(0);
    }
    std::srand(seed);
    // iterate through all sequences in input file
    for (auto & record : fin) {
        std::vector<seqan3::dna5> sequence;
        // iterate over each letter
        for (auto c : record.sequence()) {
            using namespace seqan3::literals;
            if (c == 'N'_dna5) {
                sequence.emplace_back([]() {

                    switch(rand() % 4) {
                    case 0: return 'A'_dna5;
                    case 1: return 'C'_dna5;
                    case 2: return 'G'_dna5;
                    case 3: return 'T'_dna5;
                    }
                    assert(false);
                    throw std::runtime_error("This should not be possible.");
                }());
            } else {
                sequence.emplace_back(c);
            }
        }
        fout.emplace_back(sequence, record.id());
    }

    return EXIT_SUCCESS;
}
