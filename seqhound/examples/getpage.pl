#!/usr/bin/perl
use LWP::Simple;

  $server = "http://zaphod.mshri.on.ca/";
  $path = "cgi-bin/seqhound/seqrem";
  $function = "SeqHoundProteinsFromTaxID";
  $value = "555";
  $arguments = "taxid\=".$value;
  $functioncall = "\?fnct=".$function."\&".$arguments;
  $fulladdress = $server.$path.$functioncall;
  #print "$fulladdress \n";

  $content = get($fulladdress);
  #print "$content \n";

  @lines = split(/\n/, $content);
  #print "errcode:$lines[0]\n";
  #print "values:$lines[1]\n";

  if($lines[0] =~ /ERROR/)
  {
    print "SeqHound error: $lines[0]\n";
  }
  elsif ($lines[0] =~ /NULL/)
  {
    print "SeqHound returned no value: $lines[0]\n";
  }
  elsif ($lines[1] =~ /(null)/)
  {
    print "SeqHound returned no value: $lines[0] $lines[1]\n";
  }
  else
  {
    print "OK\n";
    print "$lines[1]\n";
  }
