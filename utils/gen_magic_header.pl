#!/usr/bin/env perl

# This script is used to transform magic definition list
# in `src/magic_list' into source file `src/magic.c'

use strict;
use Data::Dumper;

my $magic_defs = "magic_defs";
my $magic_tmpl = "ss_magic.c.tmpl";
my $magic_src = "ss_magic.c";
my %magic_map;			# map of hexcode to name

open MAGIC_DEFS, "<", $magic_defs;
open MAGIC_TMPL,  "<", $magic_tmpl;
open MAGIC_SRC,  ">", $magic_src;

# resolve definitions
while (<MAGIC_DEFS>) {
  /^(\S+)\s+(\S+)$/;
  $magic_map{$1} = $2;
}
close MAGIC_DEFS;

while (<MAGIC_TMPL>) {
  print MAGIC_SRC $_;
  last if /^\s+$/;
}

my $count = 0;
my @def;
foreach my $key (sort keys %magic_map) {
  my @output = ($key =~ m/.{2}/g);
  # print Dumper \@output;
  @output = map { "0x".$_ } @output;
  my $len = scalar @output;
  push(@output, "0x00") for (0..(8 - $len - 1));
  push(@output, $len);
  push @def, "\n    {".join(", ", @output)."}";
  printf MAGIC_SRC "#define M_%s%*s0x%s\n", $magic_map{$key}, 16 - length($magic_map{$key}) + 1, " ", $key;
}

# insert into magic.c definitions
# print MAGIC_SRC "\nstatic int magic_index[MAXIDX] = {\n";
# foreach my $key (sort keys %magic_map) {

#   $key =~ /^(\S\S).*$/;
#   print MAGIC_SRC "    [0x$1] = M_$magic_map{$key};\n";
# }
# print MAGIC_SRC "};\n\n";
print MAGIC_SRC "\nstatic uint8_t magic_index[][MENTRY_LEN + 1] = {";
print MAGIC_SRC join(",", @def);
print MAGIC_SRC "\n};\n\n";


while (<MAGIC_TMPL>) {
  print MAGIC_SRC $_;
}

# print Dumper \%magic_map;

close MAGIC_TMPL;
close MAGIC_SRC;
