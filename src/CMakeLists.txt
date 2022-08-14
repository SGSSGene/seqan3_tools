cmake_minimum_required (VERSION 3.8)

add_subdirectory(oss)

add_executable (st_fastq2fasta st_fastq2fasta.cpp)
target_link_libraries (st_fastq2fasta PRIVATE seqan3::seqan3)

add_executable (st_dna5todna4 st_dna5todna4.cpp)
target_link_libraries (st_dna5todna4 PRIVATE seqan3::seqan3)

add_executable (st_index_build st_index_build.cpp)
target_link_libraries (st_index_build PRIVATE seqan3::seqan3)

add_executable (st_index_search st_index_search.cpp)
target_link_libraries (st_index_search PRIVATE seqan3::seqan3 oss)

add_executable (st_fasta_cut st_fasta_cut.cpp)
target_link_libraries (st_fasta_cut PRIVATE seqan3::seqan3)

add_executable (st_fasta_info st_fasta_info.cpp)
target_link_libraries (st_fasta_info PRIVATE seqan3::seqan3)

add_executable (st_fasta_dump st_fasta_dump.cpp)
target_link_libraries (st_fasta_dump PRIVATE seqan3::seqan3)

add_executable (st_binary_rev st_binary_rev.cpp)
target_link_libraries (st_binary_rev PRIVATE seqan3::seqan3)

add_executable (st_scheme_build st_scheme_build.cpp)
target_link_libraries (st_scheme_build PRIVATE seqan3::seqan3 generator)

add_executable (st_scheme_stat st_scheme_stat.cpp)
target_link_libraries (st_scheme_stat PRIVATE seqan3::seqan3 oss)

add_executable (st_bwt_build st_bwt_build.cpp)
target_link_libraries (st_bwt_build PRIVATE seqan3::seqan3 divsufsort64)

add_executable (st_text_map st_text_map.cpp)
target_link_libraries (st_text_map PRIVATE seqan3::seqan3)

add_executable (st_multistring_filter st_multistring_filter.cpp)
target_link_libraries (st_multistring_filter PRIVATE seqan3::seqan3)

add_executable (st_name_id_mapper st_name_id_mapper.cpp)
target_link_libraries (st_name_id_mapper PRIVATE seqan3::seqan3)

add_executable (st_compare_results st_compare_results.cpp)
target_link_libraries (st_compare_results PRIVATE seqan3::seqan3)
