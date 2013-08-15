#!/usr/bin/perl -cw

# this script reads a .mint file containing a mint network description
# using named node groups and weight matrices, and writes two files: a
# .arc file describing the network with numbered node groups and
# weight matrices, and a .h file contaning #defines that map node
# group numbers and weight matrix numbers into names. this second file
# can be include in c programs to use node group and weight matrix
# names instead of numbers.
#
# example .mint file:
# 
# network 2 1
# nodes IN 10 0
# nodes OUT 1 0
# weights IN -> OUT 0
#
# resulting .arc file:
#
# network 2 1
# nodes 10 0
# nodes 1 0
# weights 10 1 0 0 1

use strict;

my $file = shift;
my ($base, $sfx) = split( ".", $file );
my $arc = $base."arc";
my $h = $base".h";

open( FILE, shift(), "r" );
my @words = split( /\s/, <FILE> );
my $w = shift @words;
die "'network' not found" unless $w eq "network";
my $nGroups = shift @words;
my $nMatrices = shift @words;

my %groups = ();
for (1..$nGroups) {
    read_group( \%groups, \@words );
}

my %matrices = ();
for (1..$nMatrices) {
    read_matrix( \@matrices, \@words );
}

print join(" ",("network",(scalar @groups),(scalar @matrices) ) ), "\n";
for my $g (@groups) {
    

sub read_group() {
    my ($groups, $words) = @_;
    my $w = shift @$words;
    die "'nodes' not found" unless $w eq "nodes";
    my $name = shift @$words;
    my $size = shift @$words;
    my $states = shift @$words;
    $groups->{$name} = { size=>$size, states=>$states }
}

sub read_matrix() {
    my ($matrices, $words) = @_;
    my $w = shift @$words;
    die "'weights' not found" unless $w eq "weights";
    my $from = shift @$words;
    my $to = shift @$words;
    my $states = shift @$words;
    $matrices->{$from} = { to=>$to, states=>$states }
}

