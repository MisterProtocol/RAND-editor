#!/usr/bin/perl -i


while(<>) {
  chomp;
  $x = '';
  if( /^(\#else)\s+(.*)$/ && !m|/\*| ) {
    $_ = "#else /* $2 */";
  }
  print "$_\n";
}
