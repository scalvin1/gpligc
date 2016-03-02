#!/usr/bin/perl -w

# $Id: gpsp2igcfile.pl 3 2014-07-31 09:59:20Z kruegerh $
# (c) 2002-2016 Hannes Krueger
# This file is part of the GPLIGC/ogie package
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

#  This little script converts a tracklist file obtained from a Garmin GPS using
#  the gpspoint program into something resembling an IGC file for use with other programs.
#  Script by Jan M. W. Krueger, 1st of March 2004

#  put into the GPLIGC/perl CVS repository
#  $Id: gpsp2igcfile.pl 3 2014-07-31 09:59:20Z kruegerh $
#  revised by Hannes Kruger

#  usage:
#  gpsp2igc.pl  <inputfile.gpspoint

# this will write igcfiles for each day!
# with more or less igc conform filenames!
# see also gpsp2igc

# new segment on new day -> new igc file! (well, that may be problematic for australian files...)

# igc naming

$device_id = "000";
$manufacturer_short="X";
$manufacturer_long="GAR";
$filenr=1;


@lines = <STDIN>;


# flag for new track segment ( 1 is new segment)
$first = 0;
$oday ="000000"; # old day!

for($tmp=0; $tmp<=$#lines; $tmp++){
	if($lines[$tmp] =~ /trackpoint/){
		if($lines[$tmp] =~ /newsegment=\"yes\"/){
			$lines[$tmp] =~ s/\ \ newsegment=\"yes\"\ //g;
			#push(@outlines, "\n");
			#push(@igclines, "\n");
			$first = 1;
		}

		$lines[$tmp] =~ s/type\=\"trackpoint\"\ \ //g;
		$lines[$tmp] =~ s/\"/\ /g;
		$lines[$tmp] =~ s/altitude\=\ //g;
		$lines[$tmp] =~ s/latitude\=\ //g;
		$lines[$tmp] =~ s/longitude\=\ //g;
		$lines[$tmp] =~ s/unixtime\=\ //g;



		$lines[$tmp] =~ /([\d.-]+)\s*([\d.-]+)\s*([\d.-]+)\s*([\d.-]+)/;
		$alt = $1;
		$latitude = $2;
		$longitude = $3;
		$unixtime = $4;

		# Put altitude in IGC line format:
		if($alt<0){
			$altnew = -$alt;
			$altnew = sprintf "%5d", $altnew;
			$altnew =~ s/\ /0/g;
			$altnew =~ s/0/-/;
		} else {
			$altnew = sprintf "%5d", $alt;
			$altnew =~ s/\ /0/g;
		}


		# Put latitude in IGC line format:
		if($latitude < 0){
			$latitudenew = -$latitude;
			$ns = "S";
		} else {
			$latitudenew = $latitude;
			$ns = "N";
		}
		$latint = sprintf "%.0f", $latitudenew-0.5;
		$latmins = 100000* ($latitudenew - $latint)*60/100;
		$latint = sprintf "%2.0f", $latint;
		$latint =~ s/\ /0/g;
		$latmins = sprintf "%5.0f", $latmins;
		$latmins =~ s/\ /0/g;

		$latitudestring = $latint.$latmins.$ns;


		# Put longitude in IGC line format:
		if($longitude < 0){
			$longitudenew = -$longitude;
			$ew = "W";
		} else {
			$longitudenew = $longitude;
			$ew = "E";
		}
		$lonint = sprintf "%.0f", $longitudenew-0.5;
		$lonmins = 100000* ($longitudenew - $lonint)*60/100;
		$lonint = sprintf "%3.0f", $lonint;
		$lonint =~ s/\ /0/g;
		$lonmins = sprintf "%5.0f", $lonmins;
		$lonmins =~ s/\ /0/g;

		$longitudestring = $lonint.$lonmins.$ew;


		# use gmtime to get GMT from unix-epoch-seconds
		($ss, $mm, $hh, $dd, $mon, $yyyy) = gmtime ($unixtime);
		$mon++; # (dd: 1-31, mon: 0-11, y: yyyy -1900)
		$yyyy+=1900; $yy=substr($yyyy,2);


		# detect new segment and mark with HFDTEddmmyy
		if($first == 1){
			$first = 0;
				if (sprintf("%02d%02d%02d",$dd,$mon,$yy) ne $oday) {
					newfile($yyyy,$mon,$dd);
					$oday = sprintf("%02d%02d%02d",$dd,$mon,$yy);
				}
		}

		print FN "B".sprintf("%02d%02d%02d", $hh,$mm,$ss).$latitudestring.$longitudestring."A".$altnew.$altnew."\n";

	}
}


sub newfile {

	my $y=shift;
	my $m=shift;
	my $d=shift;

	my ($igcm, $igcd);
	my $igcy=substr($y,-1);

	if ($m < 10) {$igcm=$m}
	if ($m == 10) {$igcm="a"}
	if ($m == 11) {$igcm="b"}
	if ($m == 12) {$igcm="c"}

	if ($d < 10) {$igcd=$d} else {
		$igcd= chr(97+($d-10));
	}

	filenames:
 	my $fn = "$igcy$igcm$igcd$manufacturer_short${device_id}$filenr.igc";
	my $lfn= sprintf("%4d-%02d-%02d-$manufacturer_long-$device_id-%02d.igc", $y,$m,$d,$filenr);

	print "IGC file: $lfn ($fn)\n";

	if (-e $lfn) {print "$lfn ($fn) exists!\n"; $filenr++; goto filenames;}

	close FN;

	open(FN, ">$lfn") || die "cannot open $lfn for writing";

	print FN "A$manufacturer_long$device_id  ".'$Id: gpsp2igcfile.pl 3 2014-07-31 09:59:20Z kruegerh $'."\n";
	print FN "HFFTY FR Type:  unknown device\n";
	print FN "HFFSW Software:  gpspoint / gpsp2igcfile.pl\n\n";

	my $yy=substr($y,2);
	print FN "HFDTE".sprintf("%02d%02d%02d",$d,$m,$yy)."\n\n";
}
