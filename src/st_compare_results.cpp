#include <filesystem>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <sstream>


struct Reader {
    std::ifstream ifs;

    std::unordered_map<size_t, std::unordered_map<size_t, std::unordered_set<size_t>>> entries;

    Reader(std::filesystem::path file)
        : ifs(file)
    {
        while (!ifs.eof()) {
            readEntry();
        }
    }

private:
    void readEntry() {
        size_t qid, sid, spos;

        ifs >> qid; if (ifs.eof()) return;
        ifs >> sid; if (ifs.eof()) return;
        ifs >> spos; if (ifs.eof()) return;
        entries[qid][sid].insert(spos);
    }
};


void f(char symb, size_t qidx, size_t sid, size_t spos) {
    std::cout << symb << " " << qidx << " " << sid << " " << spos << "\n";
}

int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_compare_results", argc, argv};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    std::filesystem::path lhs_file{};
    parser.add_positional_option(lhs_file, "left comparison");

    std::filesystem::path rhs_file{};
    parser.add_positional_option(rhs_file, "right comparison");

    size_t error = 0;
    parser.add_option(error, '\0', "error", "allowed variation in start positions.");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }


    auto lhs = Reader(lhs_file);
    auto rhs = Reader(rhs_file);

    auto compare = [error](auto const& lhs, auto const& rhs, char symb) {
        for (auto const& [qidx, l] : lhs.entries) {
            auto iter = rhs.entries.find(qidx);
            if (iter == rhs.entries.end()) { // A complete query is missing on the right side
                for (auto const& [sid, sposs] : l) {
                    for (auto const& spos : sposs) {
                        f(symb, qidx, sid, spos);
                    }
                }
                continue;
            }
            for (auto const& [sid, sposs] : l) {
                auto iter2 = iter->second.find(sid);
                if (iter2 == iter->second.end()) {
                    for (auto spos : sposs) {
                        f(symb, qidx, sid, spos);
                    }
                    continue;
                }
                for (auto const& spos : sposs) {
                    bool found{false};
                    for (size_t e{0}; e <= error && !found; ++e) {
                        found |= iter2->second.count(spos+e) > 0;
                        found |= iter2->second.count(spos-e) > 0;
                    }
                    if (!found) {
                        f(symb, qidx, sid, spos);
                    }
                }
            }
        }
    };

    // Whats left but not right?
    compare(lhs, rhs, '<');
    compare(rhs, lhs, '>');

    return EXIT_SUCCESS;
}
