#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/std/filesystem>
#include <sstream>
#include <fstream>

void readWriteBlock(std::ifstream& ifs, std::ofstream& ofs, size_t end, int blockSize) {
    if (blockSize == 0) {
        return;
    }
    auto buffer = std::vector<char>{};
    buffer.resize(blockSize);

    ifs.read(buffer.data(), buffer.size());
    std::reverse(buffer.begin(), buffer.end());
    ofs.seekp(end-blockSize);
    ofs.write(buffer.data(), buffer.size());
}


int main(int argc, char const* const* argv) {
    seqan3::argument_parser parser{"st_rev", argc, argv};

    std::filesystem::path infile{};
    std::filesystem::path outfile{};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";
    parser.add_positional_option(infile, "Please provide a input binary file");
    parser.add_positional_option(outfile, "Please provide a output file");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    auto total_size = std::filesystem::file_size(infile);

    auto ifs = std::ifstream{infile, std::ios::binary | std::ios::in};
    auto ofs = std::ofstream{outfile, std::ios::binary | std::ios::out};

    auto size = total_size;
    while (size > 1'000'000) {
        readWriteBlock(ifs, ofs, size, 1'000'000);
        size -= 1'000'000;
    }
    readWriteBlock(ifs, ofs, size, size);




    return EXIT_SUCCESS;
}
