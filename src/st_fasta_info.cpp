// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <filesystem>
#include <seqan3/alphabet/views/all.hpp>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <sstream>

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_fasta_info", argc, argv};

    std::filesystem::path infile{};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";
    parser.add_positional_option(infile, "Please provide a fasta file.",
                                 seqan3::input_file_validator{{"fa", "fasta", "fna"}});

    std::string mode{"unknown"};
    parser.add_positional_option(mode, "Please provide a mode: names, lengths, view");

    size_t queryId{}, pos{}, len{100};
    bool revCompl{false};
    std::string qname{};

    parser.add_option(queryId, '\0', "id",        "(view) number of query");
    parser.add_option(qname,   '\0', "qname",     "(view) name of sequence (if this is set, id will be ignored)");
    parser.add_option(pos,     '\0', "start",     "(view) position where to start printing");
    parser.add_option(len,     '\0', "len",       "(view) how many chars to print");
    parser.add_flag(revCompl,  '\0', "rev-compl", "(view) consider artificial revComple (uneven numbers are artificial)");


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
    } else if (mode == "view") {
        size_t i{};
        for (auto & record : fin) {
            if (qname.empty()) {
                if (!revCompl) if (i++ != queryId) continue;
                if (revCompl) if (i != queryId and i+1 != queryId) { i+=2; continue; }
            } else {
                if (record.id() != qname) continue;
            }

            auto seq = record.sequence();
            if (revCompl and (queryId%2 != 0)) {
                seq = seq | std::views::reverse | seqan3::views::complement | seqan3::ranges::to<std::vector>();
            }

            for (size_t j{pos}; j < pos+len; ++j) {
                if (j >= seq.size()) std::cout << "_";
                else std::cout << seq[j].to_char();
            }
            std::cout << "\n";
            break;
        }

    } else {
        std::cout << "unknown mode: " << mode << "\n";
    }

    return EXIT_SUCCESS;
}
