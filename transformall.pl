#!/usr/bin/perl

my $inDir = '.\sequence';
my $outDir = '.\seqoutput';
mkdir $outDir;

opendir(DIR, $inDir) or die $!;

while (my $file = readdir(DIR)) {

	next if ($file =~ m/^\./);
	next unless ($file =~ m/\.jpg/);

	$cmd = "./kptransform.exe reference.jpg $inDir/$file replacement.jpg $outDir/$file";
	print "$cmd\n";
	print "processing $file...\n";
	system($cmd);
}

closedir(DIR);
exit 0;