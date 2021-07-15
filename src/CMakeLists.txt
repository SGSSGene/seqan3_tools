cmake_minimum_required (VERSION 3.8)

add_executable (st_fastq2fasta st_fastq2fasta.cpp)
target_link_libraries (st_fastq2fasta PRIVATE seqan3::seqan3)

add_executable (st_dna5todna4 st_dna5todna4.cpp)
target_link_libraries (st_dna5todna4 PRIVATE seqan3::seqan3)

add_executable (st_fastadump st_fastadump.cpp)
target_link_libraries (st_fastadump PRIVATE seqan3::seqan3)
