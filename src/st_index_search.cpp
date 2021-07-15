#include <seqan3/alphabet/adaptation/char.hpp>
#include <seqan3/alphabet/concept.hpp>
#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/search/fm_index/bi_fm_index.hpp>
#include <seqan3/search/search.hpp>


struct StopWatch {
    using TP = decltype(std::chrono::steady_clock::now());

    TP start{std::chrono::steady_clock::now()};

    auto now() const -> TP {
        return std::chrono::steady_clock::now();
    }

    auto reset() -> double {
        auto t = now();
        auto diff = t - start;
        start = t;
        return diff.count() / 1'000'000'000.;
    }

    auto peek() const -> double {
        auto t = now();
        auto diff = t - start;
        return diff.count() / 1'000'000'000.;
    }
};


struct my_dna4 : seqan3::sequence_file_input_default_traits_dna {
    using sequence_alphabet = seqan3::dna4;
};

struct my_dna5 : seqan3::sequence_file_input_default_traits_dna {
    using sequence_alphabet = seqan3::dna5;
};


template <typename trait>
void search_index(std::filesystem::path indexfile, std::filesystem::path queriesfile, std::filesystem::path resultfile, uint8_t errors) {
    using alphabet = trait::sequence_alphabet;

    using Index = decltype(seqan3::bi_fm_index{std::vector<std::vector<alphabet>>{}});

    StopWatch sw;
    // saving the index
    Index index{};
    {
        seqan3::debug_stream << "Loading 2FM-Index ... " << std::flush;
        std::ifstream is{indexfile, std::ios::binary};
        cereal::BinaryInputArchive iarchive{is};
        iarchive(index);
        seqan3::debug_stream << "done - took " << sw.reset() << "s\n";
    }

    auto queries = std::vector<std::vector<alphabet>>{};
    {
        seqan3::debug_stream << "Loading queries ... " << std::flush;
        auto fin = seqan3::sequence_file_input<trait>{queriesfile};
        for (auto & [seq, id, qual] : fin) {
            queries.push_back(seq);
        }
        seqan3::debug_stream << "done - loaded " << queries.size() << " queries, took " << sw.reset() << "s\n";
    }

    seqan3::debug_stream << "start searching with " << errors << " errors...\n";
    seqan3::configuration const cfg = seqan3::search_cfg::max_error_total{seqan3::search_cfg::error_count{errors}}
                                      | seqan3::search_cfg::max_error_substitution{seqan3::search_cfg::error_count{errors}}
                                      | seqan3::search_cfg::max_error_insertion{seqan3::search_cfg::error_count{errors}}
                                      | seqan3::search_cfg::max_error_deletion{seqan3::search_cfg::error_count{errors}};
    auto result = search(queries, index, cfg);
    auto result_2 = std::vector<std::tuple<int, int, int>>{};
    for (auto r : result) {
        auto qid = r.query_id();
        auto sid = r.reference_id();
        auto pos = r.reference_begin_position();
        result_2.emplace_back(qid, sid, pos);
    }
    auto delta = sw.reset();
    seqan3::debug_stream << "found " << result_2.size() << " hits in " << delta << "s which is in avg " << delta / queries.size() * 1'000'000 << "μs per query\n";
    auto ofs = std::ofstream{resultfile};
    for (auto [qid, sid, pos] : result_2) {
        ofs << qid << " " << sid << " " << pos << "\n";
    }
    seqan3::debug_stream << "Saved results in " << resultfile << "\n";
}

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_index_search", argc, argv};

    // Parser
    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path indexfile;
    std::filesystem::path queriesfile;
    std::filesystem::path outfile;

    parser.add_positional_option(indexfile,   "Please provide an index file.");
    parser.add_positional_option(queriesfile, "Please provide a fasta file with queries",
                                 seqan3::input_file_validator{{"fa", "fasta", "fna"}});
    parser.add_positional_option(outfile, "Please provide a file for the results");

    uint8_t errors{0};
    parser.add_option(errors, 'k', "errors", "Number of allowed errors.");


    bool use_dna4{false};
    parser.add_flag(use_dna4, '\0', "dna4", "Use dna 4 alphabet");


    try {
        parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }
    if (use_dna4) {
        search_index<my_dna4>(indexfile, queriesfile, outfile, errors);
    } else {
        search_index<my_dna5>(indexfile, queriesfile, outfile, errors);
    }


    return EXIT_SUCCESS;
}
