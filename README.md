ssearcher
=========

[![Build Status](https://travis-ci.org/styx-hy/ssearcher.png)](https://travis-ci.org/styx-hy/ssearcher)

another simple searcher designed for local text files

### Build

Using my Makefile (recomended), simply `make`

    make

Or using `autoconf`:

    aclocal
    autoconf
    autoheader
    automake --add-missing
    ./configure

### Algorithms

- KMP string matching
- LCRQ
  - [Fast Concurrent Queues for x86 Processors](http://dl.acm.org/citation.cfm?id=2442527)
  - Adam Morrison, Yehuda Afek [pdf](http://www.cs.tau.ac.il/~adamx/g.php?link=http://www.cs.tau.ac.il/~adamx/ppopp2013-x86queues.pdf)
    
