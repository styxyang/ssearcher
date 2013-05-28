#!/usr/bin/env perl -w

# This script is used to transform magic definition list
# in `src/magic_list' into source file `src/magic.c'

use strict;
use Data::Dumper;

my $magic_defs = "src/magic_defs";
my $magic_src = "src/magic.c";
my %magic_map;

open MAGIC_DEFS, "<", $magic_defs;
open MAGIC_SRC,  "<", $magic_src;
open MAGIC_SRCTMP,  ">", $magic_src.".tmp";

# resolve definitions
while (<MAGIC_DEFS>) {
  /^(\S+)\s+(\S+)$/;
  $magic_map{$1} = $2;
}

while (<MAGIC_SRC>) {
  print MAGIC_SRCTMP $_;
  last if /^\s+$/;
}

# insert into magic.c definitions
print MAGIC_SRCTMP "static int magic_index[MAXIDX] = {\n";
foreach my $key (sort keys %magic_map) {

  $key =~ /^(\S\S).*$/;
  print MAGIC_SRCTMP "    [0x$1] = M_$magic_map{$key};\n";
}
print MAGIC_SRCTMP "};\n\n";


while (<MAGIC_SRC>) {
  print MAGIC_SRCTMP $_;
}

print Dumper \%magic_map;

close MAGIC_DEFS;
close MAGIC_SRC;
close MAGIC_SRCTMP;
