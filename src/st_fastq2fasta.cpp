#include <sstream>

#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/std/filesystem>

int main(int argc, char ** argv)
{
    seqan3::argument_parser parser{"Fastq-to-Fasta-Converter", argc, argv};

    std::filesystem::path fastq_file{};

    // Parser
    parser.info.author = "SeqAn-Team"; // give parser some infos
    parser.info.version = "1.0.0";
    parser.add_positional_option(fastq_file, "Please provide a fastq file.",
                                 seqan3::input_file_validator{{"fq","fastq"}});
    try
    {
         parser.parse();                                                  // trigger command line parsing
    }
    catch (seqan3::argument_parser_error const & ext)                     // catch user errors
    {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n"; // give error message
        return -1;
    }

    // conversion
    {
        auto fin  = seqan3::sequence_file_input{fastq_file};
        auto fout = seqan3::sequence_file_output{std::cout, seqan3::format_fasta{}};
        fout = fin; // actual conversion
    }


    return 0;
}
