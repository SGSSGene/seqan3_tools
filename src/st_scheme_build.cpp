// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include "oss/generator/all.h"

#include <filesystem>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <sstream>

inline auto allGeneratorNames = []() -> std::string {
    if (oss::generator::all.empty()) return {};

    auto iter = oss::generator::all.begin();
    auto result = std::string{std::get<0>(*iter)};
    ++iter;
    while(iter != oss::generator::all.end()) {
        result += ", " + std::get<0>(*iter);
        ++iter;
    }
    return result;
}();

auto formatColumba(int minK, int maxK, oss::Scheme const& scheme) -> std::string {
    std::stringstream ss;

    for (size_t i{0}; i < scheme.size(); ++i) {
        auto const& s = scheme.at(i);
        ss << '{' << s.pi.at(0);
            for (auto iter = ++s.pi.begin(); iter!= s.pi.end(); ++iter) {
                ss << "," << *iter;
            }
        ss << "} ";
        ss << "{" << s.l.at(0);
            for (auto iter = ++s.l.begin(); iter!= s.l.end(); ++iter) {
                ss << "," << *iter;
            }
        ss << "} ";
        ss << "{" << s.u.at(0);
            for (auto iter = ++s.u.begin(); iter!= s.u.end(); ++iter) {
                ss << "," << *iter;
            }
        ss << "}\n";
    }
    return ss.str();
}



int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_scheme_build", argc, argv};

    std::filesystem::path infile{};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::string generatorName{""};
    parser.add_option(generatorName, 'g', "generator", "Provide a generator name, available " + allGeneratorNames);

    int minK{0};
    parser.add_option(minK, 'a', "min_k", "Minimum of errors that have to occur");
    int maxK{0};
    parser.add_option(maxK, 'k', "max_k", "Maximum allowed errors");

    int index{0};
    parser.add_option(index, 'i', "index", "Should the parts numbers starts with 0 or 1 (default: 0");


    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    auto iter = oss::generator::all.find(generatorName);
    if (iter == oss::generator::all.end()) {
        seqan3::debug_stream << "Unknown generator\n";
        return EXIT_FAILURE;
    }

    auto scheme = iter->second(minK, maxK, 4, 1'000'000);
    for (auto& s : scheme) {
        for (auto& pi : s.pi) {
            pi = pi-1+index;
        }
    }
    seqan3::debug_stream << formatColumba(minK, maxK, scheme);

    return EXIT_SUCCESS;
}
