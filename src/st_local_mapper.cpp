#include <ranges>
#include <string>

#include <seqan3/alignment/configuration/all.hpp>
#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/alphabet/adaptation/char.hpp>
#include <seqan3/alphabet/concept.hpp>
#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/alphabet/views/all.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sam_file/output.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/search/fm_index/bi_fm_index.hpp>
#include <seqan3/search/search.hpp>
#include <seqan3/utility/range/to.hpp>

auto readFasta(std::filesystem::path p) {
    auto seqs = std::vector<std::tuple<std::string, std::vector<seqan3::dna5>>>{};

    auto fin = seqan3::sequence_file_input{p};
    for (auto const& record : fin) {
        seqs.emplace_back(record.id(), record.sequence());
    }
    return seqs;
}

auto readPosition(std::filesystem::path p) {
    auto pos = std::vector<std::tuple<size_t, size_t, size_t>>{};
    auto ifs = std::ifstream{p};

    auto line = std::string{};
    while(std::getline(ifs, line)) {
        auto p0 = line.find(' ');
        if (p0 == std::string::npos) continue;
        auto p1 = line.find(' ', p0+1);
        if (p1 == std::string::npos) continue;
        auto s0 = line.substr(0, p0);
        auto s1 = line.substr(p0+1, p1);
        auto s2 = line.substr(p1+1);

	if (s0 == "_" || s1 == "_" || s2 == "_") continue;

        auto v0 = std::stod(s0);
        auto v1 = std::stod(s1);
        auto v2 = std::stod(s2);

        pos.emplace_back(v0, v1, v2);
    }

    return pos;
}

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_index_search", argc, argv};

    // Parser
    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path refFile;
    std::filesystem::path queryFile;
    std::filesystem::path positionFile;

    std::filesystem::path outFile;

    parser.add_option(refFile,      '\0', "ref", "The reference genome.");
    parser.add_option(queryFile,    '\0', "queries", "File with the queries");
    parser.add_option(positionFile, '\0', "positions", "File with the queries and their found positions");
    parser.add_option(outFile,      '\0', "output", "Output path for sam file");

    uint8_t errors{0};
    parser.add_option(errors, 'k', "errors", "Number of allowed errors.");

    bool reverseQueries{};
    parser.add_flag(reverseQueries, '\0', "reverse_queries", "Assumes every second query is a reverse complement query");


    try {
        parser.parse();
        if (outFile.empty()) {
            outFile = positionFile;
            outFile.replace_extension(".sam");
        }
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }
    auto refs    = readFasta(refFile);
    auto queries = readFasta(queryFile);

    auto pos     = readPosition(positionFile);

    auto listRefs = std::vector<std::string>{};
    auto listRefLen = std::vector<size_t>{};
    for (auto [id, seq] : refs) {
        listRefs.push_back(id);
        listRefLen.push_back(seq.size());
    }

    auto sam_out = seqan3::sam_file_output{outFile,
                                    listRefs,
                                    listRefLen,
                                    seqan3::fields<seqan3::field::seq,
                                                   seqan3::field::id,
                                                   seqan3::field::ref_id,
                                                   seqan3::field::ref_offset,
                                                   seqan3::field::alignment,
                                                   seqan3::field::flag,
                                                   seqan3::field::mapq>{}};

    for (auto [qid, sid, spos] : pos) {
        auto startPos = 0ul;
        if (spos > errors) {
            startPos = spos - errors;
        }
        auto const& [ref_id, ref_seq] = refs[sid];
        auto [q_id, q_seq] = [&]() {
            if (reverseQueries) {
                return queries[qid/2];
            }
            return queries[qid];
        }();

        seqan3::sam_flag flag{};
        if (reverseQueries && qid % 2 == 1) {
            q_seq = q_seq | std::views::reverse | seqan3::views::complement | seqan3::ranges::to<std::vector>();
            flag = seqan3::sam_flag::on_reverse_strand;
        }
        auto rhs = q_seq;
        auto endPos = std::min(ref_seq.size(), spos + rhs.size() + errors);
        auto lhs = std::vector(ref_seq.begin()+startPos, ref_seq.begin()+endPos);

        // Configure the alignment kernel.
        auto config = seqan3::align_cfg::method_global{
            seqan3::align_cfg::free_end_gaps_sequence1_leading{true},
            seqan3::align_cfg::free_end_gaps_sequence2_leading{false},
            seqan3::align_cfg::free_end_gaps_sequence1_trailing{true},
            seqan3::align_cfg::free_end_gaps_sequence2_trailing{false},
        } |  seqan3::align_cfg::edit_scheme | seqan3::align_cfg::min_score{-errors};
        auto results = seqan3::align_pairwise(std::tie(lhs, rhs), config);
        size_t i = 0;
        for (auto res : results) {
            seqan3::debug_stream << ++i << " " << q_id << " " <<  qid << " " << sid << " " << spos << " " << lhs << " " << rhs << " " << res.alignment() << " " << res.score() << "\n";
            sam_out.emplace_back(rhs,
                                 q_id,
                                 ref_id,
                                 spos,
                                 res.alignment(),
                                 flag,
                                 60u + res.score());
        }
    }

    return EXIT_SUCCESS;
}
