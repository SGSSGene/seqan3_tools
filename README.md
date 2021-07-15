# SeqAn3 Toolkit (st_*)

These are simple tools to manipulate sequence data.

Available tools:
    $ st_fastq2fasta input.fastq > output.fasta
    Converts a fastq file into a fasta file

    $st_dna5todna4 input.fasta > output.fasta
    Converts a dna5 alphabet to dna4 and replaces every occurence of 'N' with a random 'A', 'C', 'G' or 'T'.


Instructions:
1. clone this repository: `git clone --recurse-submodules https://github.com/SGSSGene/seqan3_tools`
2. create and enter build folder: `mkdir seqan3_search/build; cd seqan3_search/build`
3. run cmake with release options: `cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-mpopcnt -march=native" ..`
4. compile with `make`
