// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <filesystem>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <sstream>

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_fasta_dump", argc, argv};

    std::filesystem::path infile{};
    std::string delimiter{};
    std::string end_delimiter{};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";
    parser.add_positional_option(infile, "Please provide a fasta file.",
                                 seqan3::input_file_validator{{"fa", "fasta", "fna"}});
    parser.add_option(delimiter,     'd', "delimiter", "Delimiter between sequences");
    parser.add_option(end_delimiter, 'e', "end_delimiter", "Delimiter at the end of all sequences");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    // conversion from dna5 to dna4
    seqan3::sequence_file_input fin{infile};

    // iterate through all sequences in input file
    bool first = true;
    for (auto & record : fin) {
        if (first) {
            first = false;
        } else {
            std::cout << delimiter;
        }
        for (auto c : record.sequence()) {
            std::cout << seqan3::to_char(c);
        }
    }
    std::cout << end_delimiter;

    return EXIT_SUCCESS;
}
