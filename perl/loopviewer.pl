#!/usr/bin/perl -w


# The loopviewer-script is a tool for viewing lots of igc files as a presentation loop
# for detailed information see GPLIGC_Manual.pdf
# this is just a template, parameters in $ogieflags* have to be adjusted for your needs

#uncomment "#goto loop;" for endlss loops
#comment out one of the system calls for either 2D or 3D only

# start with
# loopviewer.pl  liste
#
# liste is a file which has one line for each igc file + comment
# "path/filename.igc"  "1.Place - Name - Class - Glider - points - speed - distance ..."



my $ogieflags= " --quiet --fullscreen --airfield-elevation=960 --spinning 0.3 --movie --cycles=1 --marker --marker-range --marker-back=700";
my $ogieflags2D = " --init-ortho-lat=48.06 --init-ortho-lon=8.8 ";
my $ogieflags3D = " --init-alt=4000 --init-heading=0 --init-lat=47.9514 --init-lon=8.765 --init-dive=-30 ";

my $file = shift;
open (INFILE,"<$file");
my @list = <INFILE>;
close INFILE;

loop:
    foreach (@list) {

	$_ =~ /^\"(.*)\"\s*\"(.*)\"/;

	#print "$2  >$1\n";
	system ("ogie $ogieflags $ogieflags3D --text \"$2\" -i \"$1\" ");
	system ("ogie $ogieflags $ogieflags2D --ortho --text \"$2\" -i \"$1\" ");


}
#goto loop;
