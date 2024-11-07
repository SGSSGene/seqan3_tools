// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <filesystem>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/io/sam_file/all.hpp>
#include <sstream>


int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_sam_info", argc, argv};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";


    std::filesystem::path in_file{};
    parser.add_positional_option(in_file, "input");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }


    auto fin  = seqan3::sam_file_input{in_file};

    auto map = std::map<size_t, size_t>{};

    auto cigarCt = std::unordered_map<char, size_t>{};

    size_t seqCt{};
    size_t charCt{};
    for (auto & record : fin) {
        seqCt += 1;
        size_t countErrors{};
        for (auto [ct, op] : record.cigar_sequence()) {
            if (op.to_char() != 'M' && op.to_char() != '=') {
                countErrors += ct;
            }
            charCt += ct;
            cigarCt[op.to_char()] += ct;
        }
        map[countErrors] += 1;
    }

    std::cout << "by error count:\n";
    for (auto [error, count] : map) {
        std::cout << count << " reads with " << error << " errors" << "\n";
    }
    std::cout << "\nby type\n";
    for (auto [c, ct] : cigarCt) {
        std::cout << c << " occurred " << ct << " times, that is " << 1.0*ct/seqCt << " per read or " << 1.0*ct/charCt << " per base\n";
    }

    return EXIT_SUCCESS;
}
