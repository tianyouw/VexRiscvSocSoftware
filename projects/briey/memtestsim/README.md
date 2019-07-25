This is similar to memtest but is meant to be run in a simulator as it flushes the cache before every read.
Also, instead of writing the entirety of heap then reading, it will write a byte or word, flush the cache, then read it.

In the makefile, VAR_SIZE=byte/word is available to adjust the size of the transfers.