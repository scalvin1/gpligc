#!/usr/bin/perl -w

# $Id: gpsp2igc.pl 3 2014-07-31 09:59:20Z kruegerh $
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
#

#  put into the GPLIGC/perl CVS repository
#  $Id: gpsp2igc.pl 3 2014-07-31 09:59:20Z kruegerh $
#
#  revised by Hannes Kruger


#  usage:
#  gpsp2igc.pl  <inputfile.gpspoint >out.igc
#  or
#  gpspoint -dt | gpsp2igc.pl >out.igc
#  should work also



@lines = <STDIN>;
@igclines = ();

# flag for new track segment ( 0 is new segment)
$first = 0;


for($tmp=0; $tmp<=$#lines; $tmp++){
	if($lines[$tmp] =~ /trackpoint/){
		if($lines[$tmp] =~ /newsegment=\"yes\"/){
			$lines[$tmp] =~ s/\ \ newsegment=\"yes\"\ //g;
			#push(@outlines, "\n");
			push(@igclines, "\n");
			$first = 0;
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
		($ss, $mm, $hh, $dd, $mon, $yy) = gmtime ($unixtime);
		$mon++; # (dd: 1-31, mon: 0-11, y: yyyy -1900)
		$yy+=1900; $yy=substr($yy,2);


		# detect new segment and mark with HFDTEddmmyy
		if($first == 0){
			push(@igclines, "HFDTE".sprintf("%02d%02d%02d",$dd,$mon,$yy)."\n");

			$first = 1;
		}

		push(@igclines, "B".sprintf("%02d%02d%02d", $hh,$mm,$ss).$latitudestring.$longitudestring."A".$altnew.$altnew."\n");

	}
}



# give out some pseudo-header
print "AXXX000 garmin2igc.pl ".'$Id: gpsp2igc.pl 3 2014-07-31 09:59:20Z kruegerh $'."\n";
print "HFFTY FR Type:  some Garmin device\n";
print "HFFSW Software:  gpspoint / garmin2igc.pl\n";

print @igclines;
