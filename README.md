# SeqAn3 Toolkit (st_*)

These are simple tools to manipulate sequence data.

Available tools:
    $ st_fastq2fasta input.fastq > output.fasta
    Converts a fastq file into a fasta file

    $ st_dna5todna4 input.fasta > output.fasta
    Converts a dna5 alphabet to dna4 and replaces every occurence of 'N' with a random 'A', 'C', 'G' or 'T'.

    $ st_dumpfasta input.fasta -d '#' -e '$' > output.txt
    Converts fasta file into a text file, where each sequence is separated by '#' and a '$' is attached at the end

    $ st_index_build --dna4 -v input.dna4.fasta output.dna4.index
    $ st_index_build -v input.dna5.fasta output.dna5.index
    Creates an 2fm-index from a fasta file


Instructions:
1. clone this repository: `git clone --recurse-submodules https://github.com/SGSSGene/seqan3_tools`
2. create and enter build folder: `mkdir seqan3_search/build; cd seqan3_search/build`
3. run cmake with release options: `cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-mpopcnt -march=native" ..`
4. compile with `make`
