# SeqAn3 Tools (st_*)

These are simple tools to manipulate sequence data.

## Available tools
  - st_fastq2fasta
    - converts fastq file to fasta file
  - st_dna5todna4
    - converts fasta with dna5 alphabet to dna4 alphabet
  - st_index_build
    - build a 2FM-Index/Bidirectional FMIndex
  - st_index_search
    - searches for given queries using an 2FM-Index
  - st_fasta_cut
    - reduces a fasta file to either a certain number of base pairs or a certain number of chromosomes
  - st_fasta_info
    - prints information about a fasta file
  - st_fasta_dump
    - dumps a fasta file into a text file with delimiter in between and at the end
  - st_binary_rev
    - reverses a binary file byte-wise without loading the complete file into memory
  - st_scheme_build
    - constructs a search scheme
  - st_scheme_stat
    - prints information about a search scheme
  - st_bwt_build
    - constructs a bwt

## Usage
    $ st_fastq2fasta input.fastq > output.fasta
    Converts a fastq file into a fasta file

    $ st_dna5todna4 input.fasta > output.fasta
    Converts a dna5 alphabet to dna4 and replaces every occurence of 'N' with a random 'A', 'C', 'G' or 'T'.

    $ st_index_build -v input.dna5.fasta output.dna5.index
    $ st_index_build --dna4 -v input.dna4.fasta output.dna4.index
    Creates an 2fm-index from a fasta file

    $ st_index_search input.dna5.index queries.dna5.fasta results.txt -k 2
    $ st_index_search --dna4 input.dna4.index queries.dna4.fasta results.txt -k 2
    Searches the index for reads provided by the queries file with 2 errors. Results are stored in results.txt

    $ st_fasta_cut --max_chr 2 input.fasta > output.fasta
    $ st_fasta_cut --max_bases 1000000 input.fasta > output.fasta
    Reduces the number of bases or chromosons in a fasta file

    $ st_fasta_info input.fasta lengths
    $ st_fasta_info input.fasta names
    Prints lengths or names of all fasta file entries

    $ st_fasta_dump input.fasta -d '#' -e '$' > output.txt
    Converts fasta file into a text file, where each sequence is separated by '#' and a '$' is attached at the end

    $ st_binary_rev input.txt > output.txt
    reverses the bytes of input.txt

    $ st_scheme_build -g "01*0_opt" -k2 > 01star0_k2.ss
    constructs a search scheme with two allowed errors, following the optimized 01*0 principle

    $ st_scheme_stat --input 01star0_k2.ss --length 100
    prints information about a search scheme: number of parts, searches, min/max errors, feasibility, uniqueness, node count, node count for edit distance.

    $ st_bwt_build --input sometext.txt > bwt.txt
    constructs the bwt of the given text.



## Build instructions
  1. clone this repository: `git clone --recurse-submodules https://github.com/SGSSGene/seqan3_tools`
  2. create and enter build folder: `mkdir seqan3_search/build; cd seqan3_search/build`
  3. run cmake with release options: `cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-mpopcnt -march=native" ..`
  4. compile with `make`
  5. (optional) put them in your PATH variable `PATH="${PATH}:path/to/seqan3_tools/build/bin"`
