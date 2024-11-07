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
#include <optional>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

auto convertDna5ToDna4(std::vector<seqan3::dna5> input) {
    for (auto& c : input) {
        using namespace seqan3::literals;
        if (c == 'N'_dna5) {
            c = []() {
                switch(rand() % 4) {
                case 0: return 'A'_dna5;
                case 1: return 'C'_dna5;
                case 2: return 'G'_dna5;
                case 3: return 'T'_dna5;
                }
                assert(false);
                throw std::runtime_error("This should not be possible.");
            }();
        }
    }
    return input;
}
auto reverseComplement(std::vector<seqan3::dna5> input) {
    std::reverse(begin(input), end(input));
    for (auto& c : input) {
        using namespace seqan3::literals;
        if (c == 'A'_dna5) c = 'T'_dna5;
        else if (c == 'C'_dna5) c = 'G'_dna5;
        else if (c == 'G'_dna5) c = 'C'_dna5;
        else if (c == 'T'_dna5) c = 'A'_dna5;
        else {
            assert(false);
            throw std::runtime_error("This should not be possible.");
        }
    }
    return input;
}

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

    size_t maxNum{std::numeric_limits<size_t>::max()};
    parser.add_option(maxNum, '\0', "num", "number of alignments to pass through");

    std::filesystem::path fastaFile{};
    parser.add_option(fastaFile, '\0', "fasta", "optional output of all the alignment as fasta reads");

    bool convertToDna4{false};
    parser.add_flag(convertToDna4, '\0', "dna4", "converts all Ns randomly to ACGT");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    auto fin  = seqan3::sam_file_input{in_file};
    auto fout = seqan3::sam_file_output{out_file};

    auto fastaOut = [&]() -> std::optional<decltype(seqan3::sequence_file_output{fastaFile})> {
        if (fastaFile.empty()) return std::nullopt;
        return seqan3::sequence_file_output{fastaFile};
    }();

    size_t count{};
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
            count += 1;
            fout.push_back(record);
            if (fastaOut) {
                auto seq = std::vector<seqan3::dna5>{record.sequence()};
                if (convertToDna4) {
                    seq = convertDna5ToDna4(std::move(seq));
                }

                using seqan3::operator""_tag;
                for (auto const& [key, value] : record.tags()) {
                    if (key == "oS"_tag) {
                        if (std::get<char>(value) == 'R') {
                            seq = reverseComplement(std::move(seq));
                        }
                    }
                }
                fastaOut->emplace_back(seq, record.id());
            }
        }
        if (count == maxNum) break;
    }

    return EXIT_SUCCESS;
}
