// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <filesystem>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <sstream>

struct Mapper {
    std::vector<std::string>                positions;
    std::unordered_map<std::string, size_t> mapLong;
    std::unordered_map<std::string, size_t> mapShort;

    Mapper(std::filesystem::path infile, bool rev) {
        auto mapfile = std::filesystem::path{infile.string() + ".map"};
        if (!exists(mapfile)) {
            seqan3::sequence_file_input fin{infile};

            // iterate through all sequences in input file
            for (auto & record : fin) {
                positions.push_back(record.id());
            }

            auto ofs = std::ofstream(mapfile.string());
            for (auto v : positions) {
                ofs << v << "\n";
            }
        } else {
            auto ifs = std::ifstream(mapfile.string());
            for (std::string pos; std::getline(ifs, pos);) {
                positions.push_back(pos);
            }
        }

        for (size_t i{0}; i < positions.size(); ++i) {
            auto n = positions[i];
            auto ns = n.substr(0, n.find(' '));
            if (!rev) {
                mapLong[n]   = i;
                mapShort[ns] = i;

            } else {
                mapLong[n] = i*2;
                mapLong[n+"_rev"] = i*2+1;
                mapShort[ns] = i*2;
                mapShort[ns+"_rev"] = i*2+1;
            }
        }
    }
    auto translateShortToId(std::string const& s) const {
        return mapShort.at(s);
    }
    auto translateLongToId(std::string const& s) const {
        return mapLong.at(s);
    }
    auto translateIdToShort(size_t i) const {
        auto n = positions[i];
        return n.substr(0, n.find(' '));
    }
    auto translateIdToLong(size_t i) const {
        return positions[i];
    }
};


int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_name_id_mapper", argc, argv};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path infile{};
    parser.add_positional_option(infile, "Please provide a fasta file.");

    bool idToName{false};
    parser.add_flag(idToName, '\0', "idToName", "reverse convertion, from name to id");

    bool revCompl{false};
    parser.add_flag(revCompl, '\0', "revCompl", "generate additional reverse complements entries with the names _rev, which are interleaved to the original data");

    bool shortNames{false};
    parser.add_flag(shortNames, '\0', "shortNames", "use short names (abort at first white space)");


    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    auto mapper = Mapper(infile, revCompl);
    for (std::string pos; std::getline(std::cin, pos);) {
        if (shortNames and !idToName) {
            auto ns = pos.substr(0, pos.find(' '));
            std::cout << mapper.translateShortToId(ns) << "\n";
        } else if (!shortNames and !idToName) {
            std::cout << mapper.translateLongToId(pos) << "\n";
        } else if (shortNames and idToName) {
            size_t i = std::stod(pos);
            std::cout << mapper.translateIdToShort(i) << "\n";
        } else if (!shortNames and idToName) {
            size_t i = std::stod(pos);
            std::cout << mapper.translateIdToLong(i) << "\n";
        } else {
            std::cerr << "This should never happen, please write a rage message to the author of this program\n";
        }
    }


    return EXIT_SUCCESS;
}
