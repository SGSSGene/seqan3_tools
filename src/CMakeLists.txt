cmake_minimum_required (VERSION 3.8)

add_executable (st_fastq2fasta st_fastq2fasta.cpp)
target_link_libraries (st_fastq2fasta PRIVATE seqan3::seqan3)
