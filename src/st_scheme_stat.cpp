#include "oss/Scheme.h"
#include "oss/nodeCount.h"
#include "oss/expand.h"

#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/std/filesystem>
#include <sstream>

auto readList(std::string_view line) -> std::vector<int> {
    std::vector<int> result;
    auto startPos = 0;
    auto endPos = line.find(',');
    while(true) {
        if (endPos > line.size()) endPos = line.size();
        auto number_view = std::string_view(line.begin() + startPos, line.begin() + endPos);
        result.emplace_back(std::stod(std::string{number_view}));
        if (endPos == line.size()) {
            break;
        }
        startPos = endPos+1;
        endPos = line.find(',', startPos);
    }

    return result;

}
auto readLine(std::string_view line) -> oss::SearchTree {
    oss::SearchTree s;
    auto startPi = line.find('{')+1;
    auto endPi = line.find('}', startPi);
    s.pi = readList(std::string_view(line.begin() + startPi, line.begin() + endPi));

    auto startL = line.find('{', startPi)+1;
    auto endL = line.find('}', startL);
    s.l = readList(std::string_view(line.begin() + startL, line.begin() + endL));

    auto startU = line.find('{', startL)+1;
    auto endU = line.find('}', startU);
    s.u = readList(std::string_view(line.begin() + startU, line.begin() + endU));

    return s;
}
auto readFile(std::filesystem::path path) -> oss::Scheme {
    auto ifs = std::ifstream{path};

    oss::Scheme result;
    std::string line;
    while (std::getline(ifs, line)) {
        result.emplace_back(readLine(line));
    }
    return result;
}


void errorPermutation(std::vector<int>& combination, int startX, std::function<void(std::vector<int> const&)> const& cb, int minK, int maxK) {
    for (int i{startX}; i < combination.size(); ++i) {
        combination[i] += 1;
        if (minK <= 0) {
            cb(combination);
        }
        if (maxK > 0) {
            errorPermutation(combination, startX, cb, minK-1, maxK-1);
        }
        combination[i] -= 1;
    }
}

void errorPermutation(int len, int minK, int maxK, std::function<void(std::vector<int> const&)> const& cb) {
    auto combination = std::vector<int>(len, 0);
    if (minK == 0) {
        cb(combination);
    }
    errorPermutation(combination, 0, cb, minK, maxK-1);
}

bool check_search(oss::SearchTree const& s, std::vector<int> const& errors) {
    int acc{0};
    for (size_t i{0}; i < s.pi.size(); ++i) {
        auto pi = s.pi.at(i);
        acc += errors[pi];
        if (acc < s.l.at(i)) {
            return false;
        }
        if (acc > s.u.at(i)) {
            return false;
        }
    }
    return true;
}
bool check_scheme(oss::Scheme const& scheme, std::vector<int> const& errors) {
    for (auto const& s : scheme) {
        if (check_search(s, errors)) {
            return true;
        }
    }
    return false;
}

int check_scheme_ct(oss::Scheme const& scheme, std::vector<int> const& errors) {
    int ct{0};
    for (auto const& s : scheme) {
        if (check_search(s, errors)) {
            ct += 1;
        }
    }
    return ct;
}


int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_scheme_stat", argc, argv};

    std::filesystem::path infile{};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path input{};
    parser.add_option(input, 'i', "input", "Provide a file with a search scheme");

    int length = 100;
    parser.add_option(length, 'l', "length", "The pattern length node count assumes (default 100)");

    int sigma = 4;
    parser.add_option(sigma, 's', "sigma", "The alphabet size node count  assumes (default 4)");


    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    auto scheme = readFile(input);
    int minK = std::numeric_limits<int>::max();
    int maxK = 0;
    int parts = 0;

    // minK and maxK
    for (auto s : scheme) {
        minK = std::min(s.l.back(), minK);
        maxK = std::max(s.u.back(), maxK);
        parts = s.pi.size();
    }

    std::vector<std::vector<int>> failedErrorPatterns{};
    std::vector<std::vector<int>> duplicateErrorPatterns{};


    errorPermutation(parts, minK, maxK, [&](std::vector<int> const& v) {
        if (!check_scheme(scheme, v)) {
            failedErrorPatterns.push_back(v);
        }
        if (check_scheme_ct(scheme, v) > 1) {
            duplicateErrorPatterns.push_back(v);
        }
    });


    auto expandedScheme = oss::expand(scheme, length);


    seqan3::debug_stream << "searches " << scheme.size()
                         << "; parts " << parts
                         << "; min_k " << minK
                         << "; max_k " << maxK
                         << "; feasible " << (failedErrorPatterns.empty()?"true":"false")
                         << "; unique "   << (duplicateErrorPatterns.empty()?"true":"false")
                         << "; node_count " << oss::nodeCount(expandedScheme, sigma)
                         << "; node_count_edit " << oss::nodeCountEdit(expandedScheme, sigma)
                         << ";\n";



/*            seqan3::debug_stream << "can't find error pattern ";
            for (auto i : v) {
                seqan3::debug_stream << i;
            }
            seqan3::debug_stream << "\n";*/



    return EXIT_SUCCESS;
}
