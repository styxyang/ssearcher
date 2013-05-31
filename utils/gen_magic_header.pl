#!/usr/bin/env perl -w

# This script is used to transform magic definition list
# in `src/magic_list' into source file `src/magic.c'

use strict;
use Data::Dumper;

my $magic_defs = "src/magic_defs";
my $magic_src = "src/magic.c";
my %magic_map;			# map of hexcode to name

open MAGIC_DEFS, "<", $magic_defs;
open MAGIC_SRC,  "<", $magic_src;
open MAGIC_SRCTMP,  ">", $magic_src.".tmp";

# resolve definitions
while (<MAGIC_DEFS>) {
  /^(\S+)\s+(\S+)$/;
  $magic_map{$1} = $2;
}
close MAGIC_DEFS;

while (<MAGIC_SRC>) {
  print MAGIC_SRCTMP $_;
  last if /^\s+$/;
}

my $count = 0;
foreach my $key (sort keys %magic_map) {
  my @output = ($key =~ m/.{2}/g);
  # print Dumper \@output;
  @output = map { "0x0".$_ } @output;
  push(@output, "0x100") for (0..(8 - $#output - 1));
  print "{".join(", ", @output)."}\n";
  printf MAGIC_SRCTMP "#define %s%*s0x%s\n", $magic_map{$key}, 16 - length($magic_map{$key}) + 1, " ", $key;
}

# insert into magic.c definitions
print MAGIC_SRCTMP "\nstatic int magic_index[MAXIDX] = {\n";
foreach my $key (sort keys %magic_map) {

  $key =~ /^(\S\S).*$/;
  print MAGIC_SRCTMP "    [0x$1] = M_$magic_map{$key};\n";
}
print MAGIC_SRCTMP "};\n\n";


while (<MAGIC_SRC>) {
  print MAGIC_SRCTMP $_;
}

# print Dumper \%magic_map;

close MAGIC_SRC;
close MAGIC_SRCTMP;
