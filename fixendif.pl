#!/usr/bin/perl -i


while(<>) {
  chomp;
  $x = '';
  if( /^(\#endif)\s+(.*)$/ && !m|/\*| ) {
    $_ = "#endif /* $2 */";
  }
  print "$_\n";
}
