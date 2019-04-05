# (c) 2001-2019 Hannes Krueger
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

package GPLIGCfunctions;

#-------------------------------------------------------------------------------

sub igc2dec {
 # converts igc-file-formatted coordinates (D)DDMMMMM(N|S|E|W) into decimal form
 # DD MM.MMM

    my $igck=shift;

    if (length($igck)==8) 	{               #Latitude DDMMMMM(N|S)
        my $D=substr($igck,0,2);
        my $M1=substr($igck,2,2);
        my $M2=substr($igck,4,3);
        my $M=$M1.".".$M2;
        my $hemissphere=substr($igck,7,1);
        my $latitude=($D+($M/60));
        if ($hemissphere eq 'S') {$latitude=$latitude*(-1);}

        return ($latitude);
    }

    if (length($igck)==9)	{		#Longitude DDDMMMMM(E|W)
        my $D=substr($igck,0,3);
        my $M1=substr($igck,3,2);
        my $M2=substr($igck,5,3);
        my $M=$M1.".".$M2;
        my $hemissphere=substr($igck,8,1);
        my $longitude=($D+($M/60));
        if ($hemissphere eq 'W') {$longitude=$longitude*(-1);}

        return ($longitude);
    }

    return ("Error in GPLIGCfunctions::igc2dec");
}


sub zan2dec {

  # converts zan-file-formatted coordinates (D)DDMMSS(N|S|E|W) into decimal form

    my $igck=shift;


    if (length($igck)==7) 	{               #Latitude DDMMSS(N|S)
        my $D=substr($igck,0,2);
        my $M=substr($igck,2,2);
        my $S=substr($igck,4,2);

        my $hemissphere=substr($igck,6,1);
        my $latitude=($D+($M/60)+($S/3600));
        if ($hemissphere eq 'S') {$latitude=$latitude*(-1);}

        return ($latitude);
    }

    if (length($igck)==8)	{		#Longitude DDDMMSS(E|W)
        my $D=substr($igck,0,3);
        my $M=substr($igck,3,2);
        my $S=substr($igck,5,2);

        my $hemissphere=substr($igck,7,1);
        my $longitude=($D+($M/60)+($S/3600));
        if ($hemissphere eq 'W') {$longitude=$longitude*(-1);}

        return ($longitude);
    }

    return ("Error in GPLIGCfunctions::zan2dec");
}


sub MaxKoor {
    # returns maximum and minimum of array (ref)

    my $arrayref = shift;

    my $max=${$arrayref}[0];
    my $min=$arrayref->[0];		# fyi: two different ways to access an element of arrayref

    foreach (@$arrayref) {
        if ($max < $_) {$max=$_;}
        if ($min > $_) {$min=$_;}
    }

    return ($max, $min);
}



sub dist {
    # calculates distance between two points on 'FAI-spheroid'
    use Math::Trig;
    my ($lat1, $lon1, $lat2 ,$lon2) = @_;
    my $rho = 6371;

    return ($rho * (acos(cos( deg2rad($lat1) ) * cos( deg2rad($lat2) ) * cos( deg2rad($lon2) - deg2rad($lon1) ) +  sin( deg2rad($lat1) ) * sin( deg2rad($lat2) ) ) ) );
}


sub setpwd {
    use File::Basename;
    return File::Basename::dirname(shift);
}

#-------------------------------------------------------------------------------

sub time2human {
### converts time (HHMMSS) to HH:MM:SS
### or date DDMMYY to DD.MM.YY
### p=t -> time , p=d -> date

    my ($time, $p) = @_;

    if ($p eq 't') {my $htime = substr($time,0,2).":".substr($time,2,2).":".substr($time,4,2); return $htime;}
    if ($p eq 'd') {my $htime = substr($time,0,2).".".substr($time,2,2).".".substr($time,4,2); return $htime;}

}

#########################################################

sub pointdist {

#calculates triangular distance of 3 points given by number in arrays of coordinates
    my ($declat, $declon, $p1, $p2, $p3) = @_;

    my $km=dist(${$declat}[$p1],${$declon}[$p1],${$declat}[$p2],${$declon}[$p2])
          +dist(${$declat}[$p2],${$declon}[$p2],${$declat}[$p3],${$declon}[$p3])
          +dist(${$declat}[$p3],${$declon}[$p3],${$declat}[$p1],${$declon}[$p1]);

    return $km;
}

#-------------------------------------------------------------------------------

sub zufall {
### random integer between $min and $max
    my ($min, $max) = @_;
    return (int(rand $max-$min+1)+$min);
}

#--------------------------------------------------------------------------------

sub zufall2 {
### random integer between $min and $max (steps of devisor)
    my ($min, $max, $devisor) = @_;
    return ( (int(rand(($max-$min+1)/$devisor))* $devisor)+$min );
}

#-------------------------------------------------------------------------------

sub takeoff_detect {

    ### detects takeoff and landing!
    my ($speedarrayref, $start_ldg) = @_;
    my $lasttime=0;
    my $times=0;

    # check average speed...
    my $speedsum = 0;
    my $counter = 0;

    foreach (@$speedarrayref) {

        if  ($_ > 5 && $_ < 500) {
            $speedsum += $_;
            $counter++;
        }
    }

    if ($counter == 0) {print STDERR "GPLIGC: Warning: no data with speed between 5 and 500 km/h!\n"; $counter=1;}
    my $avspeed=$speedsum/$counter;

    my $limit = 2;

    if ($avspeed > 30) {
        $limit = 10;
    }

    if ($avspeed > 80)  {
        $limit = 40;
    }

    # get take-off time and return
    if ($start_ldg eq 's'){
        for (my $z=0; $z<=$#$speedarrayref; $z++) {

       # 4x speed > 40  == takeoff...  (takeofftime == time of first point > 40)
            if ($speedarrayref->[$z]>$limit) {
                if ($lasttime == 1) {$times++;} else {$lasttime=1;}
                if ($times == 4) {return($z-4);}

            } else { $lasttime=0;}
        }
    }

    # get landing time and return
    if ($start_ldg eq 'l'){
        for (my $z=$#$speedarrayref; $z>=0; $z--) {
            if ($speedarrayref->[$z]>$limit) {
                if ($lasttime == 1) {$times++;} else {$lasttime=1;}
                if ($times == 4) {return($z+4);}

            } else { $lasttime=0;}
        }
    }

    # if nothing was detected (slow paraglider etc...)
    #
    #  hier muss noch ein algorithmus rein, der drachen flieger etc abdeckt...
    #  analyse der overall durchschnitsgeschwindigkeit?
    #

    if ($start_ldg eq 's') {return (0);}
    if ($start_ldg eq 'l') {return ($#$speedarrayref);}

}

#-------------------------------------------------------------------------------

sub releaseDetect {

    ### tries to detect releaseTime
    my ($altarrayref, $takeoff_index) = @_;
    my $counter = $takeoff_index;

    $takeoff_alt = $altarrayref->[$takeoff_index];

    # skip until we climbed at least 150m
    while ($altarrayref->[$counter] < $altarrayref->[$takeoff_index]+150) {
        $counter++;
        if ($counter >= $#$altarrayref) {return $takeoff_index;}
    }

    my $sinkcounter = 0;
    for (my $c = $counter; $c <= $#$altarrayref; $c++) {
        if ($altarrayref->[$c] < $altarrayref->[$c-1]) {$sinkcounter++;}
        else {$sinkcounter = 0;}

        if ($sinkcounter == 2) {return $c-2;}
    }

}

sub OxygenStatistics {

    ### determines the Oxygen use according to FAA 91.211 1l/min / 10.000ft
    my ($altarrayref, $dectimearrayref, $qnh) = @_;

    my $time_100=0;
    my $time_125=0;        # minutes total
    my $time_125acc=0;        # minutes total
    my $time_140=0;
    my $time_180=0;

    my $limit_100=3028;
    my $limit_125=3810;
    my $limit_140=4267; # 14.000ft
    my $limit_180=5486;  #FL180

    my $free_flag = 1; # once above FL125 there is no free time between 100 an 125 any more
    my $free_time=0;
    my $maxalt=0;
    my $maxpalt=0;

    my $liters_100=0;
    my $liters_125=0;
    my $liters_125acc=0;
    my $liters_140=0;
    my $liters_180=0;

    #loop
    for (my $c = 0; $c <= $#$altarrayref; $c++) {

        # check max
        if ($altarrayref->[$c] > $maxalt) {$maxalt=$altarrayref->[$c]}
        if (altitude(pressure($altarrayref->[$c],$qnh),1013.25) > $maxpalt) {$maxpalt=altitude(pressure($altarrayref->[$c],$qnh),1013.25)}

        # recommended liumit FL100
        if (altitude(pressure($altarrayref->[$c],$qnh),1013.24) > $limit_100 && altitude(pressure($altarrayref->[$c],$qnh),1013.24) <= $limit_125) {

            $time_100 += ($dectimearrayref->[$c+1]-$dectimearrayref->[$c]);
            $liters_100 +=  (($dectimearrayref->[$c+1]-$dectimearrayref->[$c])*60) * ((altitude(pressure($altarrayref->[$c],$qnh),1013.24)/0.3048)/10000);
        }

        #   softlimit 125
        if (altitude(pressure($altarrayref->[$c],$qnh),1013.24) > $limit_125 && altitude(pressure($altarrayref->[$c],$qnh),1013.24) <= $limit_140) {

            if ($free_flag) {
                if ($free_time < 0.5) {
                    $free_time += $dectimearrayref->[$c+1]-$dectimearrayref->[$c];
                    $free_flag=0 if ($free_time >= 0.5);
                }
            } else {
                $time_125acc += ($dectimearrayref->[$c+1]-$dectimearrayref->[$c]);
                $liters_125acc += (($dectimearrayref->[$c+1]-$dectimearrayref->[$c])*60) * ((altitude(pressure($altarrayref->[$c],$qnh),1013.24)/0.3048)/10000);
            }

            $time_125 += ($dectimearrayref->[$c+1]-$dectimearrayref->[$c]);
            $liters_125 += (($dectimearrayref->[$c+1]-$dectimearrayref->[$c])*60) * ((altitude(pressure($altarrayref->[$c],$qnh),1013.24)/0.3048)/10000);
        }

        if (altitude(pressure($altarrayref->[$c],$qnh),1013.24) > $limit_140 && altitude(pressure($altarrayref->[$c],$qnh),1013.24) <= $limit_180) {
            $free_flag=0;
            $time_140 += ($dectimearrayref->[$c+1]-$dectimearrayref->[$c]);
            $liters_140 += (($dectimearrayref->[$c+1]-$dectimearrayref->[$c])*60) * ((altitude(pressure($altarrayref->[$c],$qnh),1013.24)/0.3048)/10000);
        }

        if (altitude(pressure($altarrayref->[$c],$qnh),1013.24) > $limit_180) {
            $free_flag=0;
            $time_180 += ($dectimearrayref->[$c+1]-$dectimearrayref->[$c]);
            $liters_180 += (($dectimearrayref->[$c+1]-$dectimearrayref->[$c])*60) * ((altitude(pressure($altarrayref->[$c],$qnh),1013.24)/0.3048)/10000);
        }
    }

    $free_time = 0.5 if  ($free_time > 0.5);

    #print "Oxygen statistics:\n";
    #print "above $softlimit m  (min): ".($time_above_sl*60)."\n";
    #print "above $hardlimit m  (min): ".($time_above_hl*60)."\n";
    #print "Liters to be used :".$liters_FAA." ".$liters_Oxymizer."\n";

    return ($maxalt,$maxpalt,$time_100,$time_125,$time_125acc,$time_140,$time_180,$liters_100,$liters_125,$liters_125acc,$liters_140,$liters_180,$free_time);

}

#-------------------------------------------------------------------------------

sub dec2time {
### converts decimal time in (h)h:mm:ss
    my $dectime= shift;

    my $hh = int $dectime;
    my $rest = $dectime - int $dectime;
    my $mm = int(60 * $rest);
    $rest= 60 * $rest - $mm;
    my $ss = int (60*$rest);

    $rest=60 * $rest -$ss;
    if ($rest > 0.5) {$ss++;}

    if ($ss == 60) {$ss=0;$mm++;}

    # why was hh not 0 padded?
    # I added this now 100809, side effects?

    if (length($hh) == 1) {$hh = "0"."$hh";}
    if (length($mm) == 1) {$mm = "0"."$mm";}
    if (length($ss) == 1) {$ss = "0"."$ss";}

    return ("$hh:$mm:$ss");

}

#-------------------------------------------------------------------------------

sub Ausschnitt2 {
    # only for zoomfunction of FlightViewWindow!
    # returns window boundaries for zoom (centred at lat/lon using zoom_sidelength (kl))

    use Math::Trig;
    my ($height, $width, $lat, $lon, $kl) = @_;
    my $halbkilometer=0.004496608; # 1/2 km in deg on great circle
    my $sideratio=$width/$height;

    my $xmin=$lon-($kl*$halbkilometer* ( 1/cos(deg2rad($lat)) ));
    my $xmax=$lon+($kl*$halbkilometer* ( 1/cos(deg2rad($lat)) ));
    my $ymin=$lat-($kl*$halbkilometer / $sideratio );
    my $ymax=$lat+($kl*$halbkilometer / $sideratio );

    return ($xmax, $xmin, $ymax, $ymin);
}

#-------------------------------------------------------------------------------

# calculates data for cylinders viewing in FVW...
sub zylinder2 {

    use Math::Trig;
    my ($lat, $lon, $r, $startangle, $angularrange) = @_; ###r=durchmesser
    my $gpx;

    my $start = deg2rad($startangle);
    my $range = deg2rad($angularrange);

    my @dlat=();
    my @dlon=();

    $gpx= $r * 0.0089932161;

    my $step = 0.0314;

    for (my $a=$start;$a<=$start+$range;$a=$a+$step) {    # step 0.0314 gives 200 points on full circle
        push(@dlat, cos($a)*$gpx);
        push(@dlon, sin($a)*$gpx*(1/cos(deg2rad($lat))));
    }

    return (\@dlat, \@dlon);

} ###ende zylinder2 andere version fuer FVWindow!

#-------------------------------------------------------------------------------

# calculates sectors or starting line...
#

# NOT NEEDED, TO BE DELETED..........

sub sector {

    use Math::Trig;

    my ($n, $lat, $lon, $lat_after, $lon_after, $lat_before, $lon_before)= @_;

    # n = 0  ->  starting point
    # n = 1 ->   finish
    # n = 2 ->    wp
    # n < 5  ->  n km long starting-line

    my $direction

}

#-------------------------------------------------------------------------------

#Calculates coordianates of a point which is in direction, distance
# from a given point

sub go {

    use Math::Trig;
    my ($lat, $lon, $heading, $distance) = @_;

    #distance needs to be in degrees...

    my $c =
      (360 / (6371 * 2 * 3.141592654) ) * $distance ;

    my $a = 90 - $lat;

    my $beta = $heading;

    # berechnung nach Seitenkosinussatz
    my $b = rad2deg(acos(
            cos(deg2rad($c)) * cos(deg2rad($a))
              + sin(deg2rad($c)) * sin(deg2rad($a)) * cos(deg2rad($beta))

          ));

    my $gamma = rad2deg(asin(
            (   sin(deg2rad($c)) / sin(deg2rad($b))  ) * sin(deg2rad($beta))
          ));

    return ( 90 -$b , $lon + $gamma);

}

#-------------------------------------------------------------------------------

sub sectordirection {

    #berechnet den gegenkurs zur winkelhalbierenden

    my ($head1, $head2) = @_;

    if ($head1 < $head2) {
        my $zw = $head1;
        $head1 = $head2;
        $head2 = $zw;
    }

    my $delta = $head1 - $head2;

    if ($delta > 180) {
        $delta = 360 - $delta;

        #print "d < 180 ! \n";
        return gegenkurs($head1)+$delta/2;
    } else {

        #print "nicht groesser \n";

        my $secdir =  gegenkurs($head1)-$delta/2;
        if ($secdir < 0) { $secdir = $secdir + 360; }
        return $secdir;

    }

}

#-------------------------------------------------------------------------------

sub gegenkurs {
    my $kurs = shift;
    my $gegenk = $kurs - 180;
    if ($gegenk < 0) {$gegenk = $gegenk + 360;}

    return $gegenk;
}

#-------------------------------------------------------------------------------

sub kurs {

    # calculates the heading from point 1 to point 2
    # at point 1
    #
    #          IV     I
    #       -------------
    #          III    II
    #

    my ($lat1, $lon1, $lat2, $lon2) = @_;

    my $b = 90 - $lat1;
    my $a = 90 - $lat2;

    my $gamma = $lon1 - $lon2;

    if ($gamma < 0) { $gamma = $gamma * -1} ;

    my $c = rad2deg(acos(
            cos(deg2rad($a)) * cos(deg2rad($b))
              + sin(deg2rad($a)) * sin(deg2rad($b)) * cos(deg2rad($gamma))
          ));

    my $alpha = 0;

    if ($c != 0) {  # c == 0 if there is no movement ?
        my $zwischenrechnung = (sin(deg2rad($a)) / sin(deg2rad($c)) )  * sin(deg2rad($gamma));

        if ($zwischenrechnung > 1) {$zwischenrechnung =1;}

        $alpha = rad2deg(asin($zwischenrechnung));

    }

    if ($lat1 >= $lat2 && $lon2 >= $lon1) { $alpha = 180-$alpha ; goto endmarke;}  # quadrant 2

    if ($lat1 >= $lat2 && $lon2 <= $lon1) { $alpha = 180+$alpha ; goto endmarke;}  # quadrant 3

    if ($lat1 <= $lat2 && $lon1 >= $lon2) { $alpha = 360-$alpha ; goto endmarke;}  # quadrant 4

    endmarke:

    return $alpha;
}

#-------------------------------------------------------------------------------

sub coorconvert {

    # converts decimal coordinates into different formats
    my ($cor, $lorl, $outformat)=@_;

    #lorl = lat _ or _ lon
    #outformat:
    # 'igc' 'igch' 'zan' 'zanh'

    my $wesn;
    my $deg_digits;

    if ($outformat eq 'deg') {
        return (sprintf("%.6f",$cor));
    }

    # determination of hemisphere s,n,w,e
    if ($lorl eq 'lat') {
        $deg_digits=2;
        if (substr($cor,0,1) eq '-') {
            $wesn='S'; $cor=$cor*(-1);
        }
        else
        {$wesn='N'; }

    }

    if ($lorl eq 'lon') {
        $deg_digits=3;
        if (substr($cor,0,1) eq '-') {
            $wesn='W'; $cor=$cor*(-1);
        }
        else
        {$wesn='E'; }
    }

    my $deg = int $cor; 					#degrees

    while ((length $deg) < $deg_digits) { $deg = "0".$deg;}

    #pre-zeros

    my $minigc =  ($cor - $deg)*60;				#minutes decimal
    my $minzan = int (($cor - $deg)*60);			#only minutes (integer)

    if ((length $minzan) < 2) {$minzan="0".$minzan;}	#zanderformatminutes gets filled up with zeros to 2digits

    my $seczan = int(($minigc - $minzan)*60);		#only seconds

    #print "laenge ".(length $seczan)."\n";

    my $rest_zan_sec=((($minigc-$minzan)*60)-$seczan);	#rest after decimal

    #print "restzansecs $rest_zan_sec \n";

    if ($rest_zan_sec > 0.5) {$seczan++;}

    #round up seconds if necessary

    #print "igc min :   $minigc \n";

    my $igc_min_3stell = sprintf("%.3f",$minigc);		#igc-minutes m.mmm

    #print "igc_3stell $igc_min_3stell \n";

    if ((substr($igc_min_3stell,2,1)) ne '.') {$igc_min_3stell="0".$igc_min_3stell;}

    my $rest_igc_min = int($minigc * 1000);			#cutted rest

#if ($rest_igc_min > 0.5) { $igc_min_3stell += 0.001;print "igc gerundet\n";}	#round up if necessary

    if ((length $seczan) < 2) {$seczan="0".$seczan;}

    if ($outformat eq 'igc') {
        my $igc_min_without_decimal = substr($igc_min_3stell,0,2).substr($igc_min_3stell,3,3);
        return ("$deg$igc_min_without_decimal$wesn");
    }
    if ($outformat eq 'igch') {
        return ("${deg} $igc_min_3stell\' $wesn");
    }
    if ($outformat eq 'zan') {
        return ("$deg$minzan$seczan$wesn");
    }
    if ($outformat eq 'zanh') {
        return ("${deg} $minzan\' $seczan\" $wesn");
    }

    return ("no valid outformat in GPLIGCfunctions::coorconv!!! $outformat");

}

#-------------------------------------------------------------------------------

sub pressure {

# calculates the pressure for given altitude and reference pressure, Std-Atmosphere
    my $stdpress = 1013.25;
    my $alt = shift;
    my $p0 = shift;
    if (defined $p0 && $p0 != 0) {$stdpress=$p0;}

    return ( $stdpress * (1-(0.0065*$alt/288.15))**5.255 );
}

sub referencepressure {

# calculates the referencepressure p0 for given altitude and pressure, Std-Atmosphere
#	my $stdpress = 1013.25;
    my $alt = shift;
    my $p = shift;

    #	if (defined $p0 && $p0 != 0) {$stdpress=$p0;}

    return (   $p / ((1-(0.0065*$alt/288.15))**5.255) );
}

sub altitude {

# calculates an altitude from given pressure and reference pressue, std-atmosphere
    my $stdpress = 1013.25;
    my $p = shift;
    my $p0 = shift;
    if (defined $p0 && $p0 != 0) {$stdpress=$p0;}

    return ((( 1-($p/$stdpress)**(1/5.255) ) * 288.15)/0.0065);
}

#-------------------------------------------------------------------------------

sub time2dec {
    my $time = shift;
    my $H=substr($time,0,2);
    my $M=substr($time,2,2);
    my $S=substr($time,4,2);
    if (!defined $H || !defined $M || !defined $S) {
        print "wrong string in time2dec: >$time<\n";
    }
    return ($H+(($M+($S/60))/60));
}

#-------------------------------------------------------------------------------

sub retArrayRef {

    #anonymous arrayreferenz erzeugen
    my @Array=();
    return \@Array;
}

#-------------------------------------------------------------------------------

# this will evaluate the time of a JPEG from exif.
# single argument is a jpeg-file and return value is decimal time
# or zero in case of an error
sub getJPEGtime {
    my $file = shift;
    require Image::ExifTool;

    my $info = Image::ExifTool::ImageInfo($file);

    # for full exif tags output!
    #	while ( ($k,$v) = each %$info ) {
    #	print "$k => $v\n";
    #	}

    if (!defined $info) {
        print "GPLIGCfunctions::getJPEGtime: cant open file $file!\n";
        return 0;
    }

    #my $image = $exif -> get_image_info();
    #    my $camera = $exif -> get_camera_info();
    #    my $other = $exif -> get_other_info();
    #    my %complete = (%$other, %$camera, %$image); # merging hashes into one
    #my %complete = (%$info); # merging hashes into one
    #print $complete."\n";

    my $val;
    my @times = ();

    foreach $val ("DateTimeOriginal","CreateDate","Image Created", "Image Generated", "Image Digitized",
        "DateTime", "Date Time", "Date and Time","ModifyDate", "FileModifyDate") {

        if (exists $info->{$val}) {

            #		print $val." found: =>";
            if ($info->{$val} =~ /(\d+):(\d+):(\d+)\s+(\d+):(\d+):(\d+)/ ){
                push (@times, "$4$5$6");

                #		print "$4$5$6 \n";
            }
        }
    }

    # now we return the first value we found...
    # maybe we can check here for multiple different times? ===15===
    return (time2dec($times[0])) if ($#times >= 0);
    if ($#times < 0) { return 0;}
}

# very simple geotagging!
sub geotag {

    require Image::ExifTool;
    my $file = shift;
    my $lat = shift;
    my $lon = shift;
    my $alt = shift;
    my $decutc = shift;
    my $tzshift = shift;
    my $force_overwrite = shift;

    my $tagsset = 0;
    my $succ=0;
    my $err="no error";

    #print "geotag: $file to be treated\n";

    #maybe we should update the time too?

    my $thingy = new Image::ExifTool;

    $tagsset += $thingy -> SetNewValue('GPSLatitude', $lat);
    $tagsset += $thingy -> SetNewValue('GPSLongitude', $lon);

    $tagsset += $thingy -> SetNewValue('GPSLatitudeRef', 'N');
    $tagsset += $thingy -> SetNewValue('GPSLongitudeRef', 'E');

    if ($lat < 0) { $thingy -> SetNewValue('GPSLatitudeRef', 'S'); }
    if ($lon < 0) { $thingy -> SetNewValue('GPSLongitudeRef', 'W'); }

    $tagsset += $thingy -> SetNewValue('GPSAltitude', $alt);

    # complains...
    #$thingy -> SetNewValue('GPSAltitudeRef', '0');

    # write GPS timeStamp
    while ($decutc > 24) {$decutc=$decutc-24;}
    $tagsset += $thingy -> SetNewValue('GPSTimeStamp', $decutc);

    my $local=$decutc+$tzshift;
    while ($local < 0 ) {$local=$local+24;}
    while ($local > 24) {$local=$local-24;}

    print "geo-tag: not enough tags set: $tagsset\n" if ($tagsset < 6) ;

    # intercept error messages!

    # check for existence of tags! HERE
    if ($force_overwrite == 0) {
        my $thingx = new Image::ExifTool;
        my $info = $thingx->ImageInfo($file, 'GPSLatitude','GPSLongitude','GPSAltitude','GPSTimeStamp');
        foreach (sort keys %$info) {

            #print "$_ => $$info{$_}\n";
            if ($_ eq "GPSLatitude" || $_ eq "GPSLongitude" ||$_ eq "GPSAltitude" || $_ eq "GPSTimeStamp") {
                return 2; # tags exists!
            }
        }
    }

    ($succ, $err) = $thingy ->WriteInfo($file);

    if ($succ > 0) {return 1};
    print "geo-tag: error: $err \n";
    return 0;
}

sub getFiletime {

    #require File::stat;
    my $file = shift;

    #	print "GetFileTime: $file\n";
    $x= (stat($file))[9];
    my ($ss,$mm,$hh) = localtime($x);

    #print "$hh : $mm : $ss \n";
    my $ret = sprintf("%02d%02d%02d",$hh,$mm,$ss);

    #print "$ret<===\n";
    return(time2dec($ret));
}

#-------------------------------------------------------------------------------
sub get_name_date {
    my ($ss, $mm, $hh, $dd, $mon, $yyyy) = gmtime ();
    $mon++; # (dd: 1-31, mon: 0-11, y: yyyy -1900)
    $yyyy+=1900;
    $lfn= sprintf("%4d-%02d-%02d", $yyyy,$mon,$dd);
    return ($lfn);
}

sub guessdatefromfilename {
    my $fn = shift;
    my $y = 0;
    my $m = 0;
    my $d = 0;

    my ($name, $p, $suf) = File::Basename::fileparse($fn);
    if (length($name) == 12 && lc(substr ($name,-3)) eq "igc") {
        if ( substr($name,0,1) =~ /^\d+$/ ) {$y += substr($name,0,1);
            $y+=2010;
        }
        if ( substr($name,1,1) =~ /^\d+$/ ) {$m = substr($name ,1,1)}
        else {
            $m = ord(lc(substr($name,1,1)))-87;
            $m = 1 if ($m<10 || $m>12);
        }
        if ( substr($name,2,1) =~ /^\d+$/ ) {$d = substr($name ,2,1)}
        else {
            $d = ord(lc(substr($name,2,1)))-87;
            $d = 1 if ($d<10 || $d>31);
        }
    }

    # igc long format
    if (length($name) == 25 && lc(substr ($name,-3)) eq "igc") {
        if ( substr($name,0,4) =~ /^\d+$/ ) {$y = substr($name,0,4)}
        if ( substr($name,5,2) =~ /^\d+$/ ) {$m = substr($name,5,2)}
        if ( substr($name,8,2) =~ /^\d+$/ ) {$d = substr($name,8,2)}
    }

    #printf("%4d-%02d-%02d \n", $y,$m,$d);
    return ($y,$m,$d);
}

# some stuff for maps:
# code from
# http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
sub getTileNumber {
    my ($lat,$lon,$zoom) = @_;
    my $xtile = int( ($lon+180)/360 *2**$zoom ) ;
    my $ytile = int( (1 - log(tan(deg2rad($lat)) + sec(deg2rad($lat)))/pi)/2 *2**$zoom ) ;
    return ($xtile, $ytile);
}
sub Project {
    my ($X,$Y, $Zoom) = @_;
    my $Unit = 1 / (2 ** $Zoom);
    my $relY1 = $Y * $Unit;
    my $relY2 = $relY1 + $Unit;

# note: $LimitY = ProjectF(degrees(atan(sinh(pi)))) = log(sinh(pi)+cosh(pi)) = pi
# note: degrees(atan(sinh(pi))) = 85.051128..
#my $LimitY = ProjectF(85.0511);

    # so stay simple and more accurate
    my $LimitY = pi;
    my $RangeY = 2 * $LimitY;
    $relY1 = $LimitY - $RangeY * $relY1;
    $relY2 = $LimitY - $RangeY * $relY2;
    my $Lat1 = ProjectMercToLat($relY1);
    my $Lat2 = ProjectMercToLat($relY2);
    $Unit = 360 / (2 ** $Zoom);
    my $Long1 = -180 + $X * $Unit;
    return ($Lat2, $Long1, $Lat1, $Long1 + $Unit); # S,W,N,E
}
sub ProjectMercToLat($){
    my $MercY = shift;
    return rad2deg(atan(sinh($MercY)));
}
sub ProjectF
{
    my $Lat = shift;
    $Lat = deg2rad($Lat);
    my $Y = log(tan($Lat) + sec($Lat));
    return $Y;
}

1;
