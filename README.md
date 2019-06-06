# myfw
C implementation of word frequency - written in collaboration with Ryan Premi

Compile with make
This program will take in a file, or multilpe files and returns the top 10 most frequent words and their count

Usage: ./myfw [-n] <num> [files]

If no files are given, then myfw will read from stdin.
If the -n flag is given, then the user may specify how many words are listed at the end.
This implementation uses a modified hashtable to store the words and their frequencies.
