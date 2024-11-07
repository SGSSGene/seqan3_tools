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
    seqan3::argument_parser parser{"st_fastq2fasta", argc, argv};

    std::filesystem::path fastq_file{};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";
    parser.add_positional_option(fastq_file, "Please provide a fastq file.",
                                 seqan3::input_file_validator{{"fq","fastq", "fa"}});
    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    // conversion
    {
        auto fin  = seqan3::sequence_file_input{fastq_file};
        auto fout = seqan3::sequence_file_output{std::cout, seqan3::format_fasta{}};
        fout = fin; // actual conversion
    }


    return EXIT_SUCCESS;
}
