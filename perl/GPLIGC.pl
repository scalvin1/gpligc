#!/usr/bin/perl -w

# version and copyleft
my $version='1.11';
my $copyright_years="2000-2021";
my $copyright="(c) $copyright_years by Hannes Krueger";

# new version welcome message
my $new_version_message=<<ENDE;
News for version $version

 - google satellite layers fixed (URL updated)
 - openflightmaps can be used (local installation of maps needed)

for details see ChangeLog and read the Manual

If you're using GPLIGC/ogie on a regular basis,
please consider to subscribe to the gpligc-announce
mailinglist (see menu 'about')

* to get rid of this message, choose 'save configuration'
  from menu 'options' *
ENDE

# set default configuration settings (configuration-hash...)
# needs some order ? alphabetic/functional groups?
my %config = ();
$config{'new_version_message_shown'}="0.1";
$config{'photos'}=1; 				#use photos?
$config{'photo_path'}="none";
$config{'picture_viewer'} = "internal";		#internal, or ext app
$config{'mm_player'} = "mplayer"; 		#multimedia
$config{'skip_check'} = 1;			#show gaps in track?
$config{'skip_limit_minutes'} = 1.0;		#actually, these are minutes now! 0.1 = 6sec
$config{'skip_del_first_after'}=1; 		#first fix after skip will be deleted!
$config{'terminal'}="xterm -hold -e";
$config{'garmin_download'}="sudo gpspoint -dt -p /dev/ttyUSB2";
$config{'gpsbabel_tdownload'}="sudo gpsbabel -t -i garmin -f /dev/ttyUSB2 -o igc -F ";
$config{'geotag_force_overwrite'}= 0;
$config{'mm_mountpoint'}="/mnt/sdC";
$config{'mm_download_dirs'}="Aufnahmen Fotos Videoclips";
$config{'browser'} = "/usr/bin/firefox";
$config{'gnuplot_win_exec'} = "wgnuplot.exe";
$config{'gnuplot_major_version'} = 4;
$config{'gnuplot_4_terminal'} = 0;
$config{'gnuplot_terminal_app'} = "xterm -e";
$config{'gnuplot_3d_rotx'} = 30;		# values for gnuplot 3d-view
$config{'gnuplot_3d_rotz'} = 330;
$config{'te_vario_fallback'} = 1;		# calculate TE vario from groundspeed?
$config{'te_warning'}=0;			# set to 0 to avoid TE from groundspeed warning
$config{'wind_analysis'} = 1;			# wind analysis with f7 stat
$config{'optimizer_cycles_mmc'} = 20;
$config{'optimizer_cycles_sa'} = 5;
$config{'optimizer_method'}="mmc";
$config{'optimizer_mmc'}=" -m 1000 -mmc 25000 -devisor 3 -refine 2 ";
$config{'optimizer_sa'}= " -sima -m 1000 -sacycles 500 -saexp -sapara 15.0 -saparb 0.03 -devisor 3 -refine 2 ";
$config{'optimizer_verbose'} = 0;
$config{'optimizer_debug'} = 0;
$config{'maps'} = "1";
$config{'map_type'}= "osm";
$config{'maps_zoomlevel'}= 8;
$config{'map_max_scalesize'}=750;
$config{'map_max_tiles'}=30;

# set path for slippery map tiles cache
if ($^O eq "MSWin32") {
    if (defined $ENV{'GPLIGCHOME'}) {
        $config{'map_path'} = $ENV{'GPLIGCHOME'}."/map/";
    } else {
        $config{'map_path'} = "C:/GPLIGC/map/";
    }
} else {
    $config{'map_path'} = "$ENV{'HOME'}/.gpligc/map/";
}

$config{'vertical_speed_unit_name'} = "m/s";	# to add more units edit the options-menu
$config{'vertical_speed_unit_factor'} = 1.0;
$config{'speed_unit_name'} ="km/h";
$config{'speed_unit_factor'} = 1.0;
$config{'altitude_unit_name'} ="m";
$config{'altitude_unit_factor'} = 1.0;
$config{'distance_unit_name'} ="km";
$config{'distance_unit_factor'} = 1.0;
$config{'waypoint_linewidth'} = 3.0;
$config{'cylinder_linewidth'} = 3.0;
$config{'task_linewidth'} = 3.0;
$config{'marker_linewidth'} = 3.0;
$config{'fvw_grid'}='yes';
$config{'fvw_baro_grid'}='yes';
$config{'fvw_baro_fraction'}=3;
$config{'open_additional_info'} = 0;		# open the additional info window automatically, if no gpi file exists
$config{'integrate_over'} = 10;			# how many data-points are averaged for integration of...
$config{'vario_histo_intervall'} = 0.5;
$config{'speed_histo_intervall'} = 5;
$config{'baro_histo_intervall'} = 50;
$config{'baro_grid_small'} = 500;
$config{'baro_grid_large'} = 1000;
$config{'draw_task'}=0;
$config{'draw_wpcyl'}=0;
$config{'draw_accuracy'}=1;
$config{'viewclick_res'}=1;
$config{'timezone'} = "0"; 			# timzone offset from UTC to camera-time (and localtime?)
# removed from config not really needed
$libdir = "zzLIBDIRzz";  			# LIBDIR PATH will be changed by the installation
$datadir = "zzDATADIRzz"; 	 		# DATADIR PATH will be changed by the installation
$config{'gnuplot_draw_style'}="with lines";
$config{'gnuplot_grid_state'}='set grid';
$config{'gnuplot_terminal'}="x11";
$config{'zylinder_radius'}=0.5;			# now in km (09/2015)
$config{'zylinder_wp_type'}="both";
$config{'starting_line'}=10;			# starting line length
$config{'zylinder_names'}=1;
$config{'zoom_sidelength'}="10";		# in km
$config{'zoom_border'}="3";		# in km
$config{'ENL_noise_limit'}=500;
$config{'coordinate_print_format'}="igch"; 	# igch, zanh, deg
$config{'DEBUG'} = 0;
$config{'window_height'}=500;			#this is initial height/width of frame_canvas
$config{'window_width'}=900;

my $text=<<ENDE;
GPLIGC Version $version   Copyright (c) $copyright_years by
Hannes Krueger <Hannes.Krueger\@gmail.com> and Jan MW Krueger

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

ENDE

### Global variables
$altmode = 'baro';
$filestat='closed';
$file='No file opened';
#$sigusr2flag=0;
$optimized_for = "none";
$zoom = 0;
$cut_track = 0;
$hide_mm = 0; 		# if 1 hide pictures and mm
$delta_photo_time = 0;	# offset between GPS time and camera clock
$savname = 'no';		# not set
$zl = 0;
$tiles = 0;
$map_reload = 0;

# flag for Tk::JPEG  module
my $have_jpeg = 0;

# flag for Image:ExifTool
my $have_EXIF = 0;

# flag for Imager
my $have_imager = 0;
my $have_png = 0;

$gpi{'qnh'} = 1013.25; 	# just to avoid an error if the pressure plot is issued without igc file

# all temp-files to be cleaned later
# these are the data files used for Gnuplot
@clearlist=qw(zyl.dat speed.dat press.dat plot.bat vario.dat gpsvario.dat deltah.dat
  baro.dat gpsalt.dat 3d.dat lonproj.dat latproj.dat befehl.dat
  km.dat lonlat.dat enl.dat ias.dat tas.dat iashisto.dat tashisto.dat shisto.dat vhisto.dat
  vhistogps.dat plot.gpl plot.bat barohisto.dat gpshisto.dat wind.dat sat.dat tmp.png);

# ===16===  is this all needed... socket connection!!!
# this is used for Communication with openGLIGCexplorer
# use IPC::SysV qw(IPC_RMID IPC_CREAT S_IRWXU ftok);
# use SysVIPC on non-windows platforms only
# BEGIN {} is executed at compile time...
# OLD CODE...
BEGIN {
    if ($^O ne "MSWin32") {
#        require IPC::SysV;
#        import IPC::SysV qw(IPC_RMID IPC_CREAT S_IRWXU ftok);
    } else {
        require Win32;
        import Win32 qw(GetOSName);
    }
}

# Tk-Module is needed!
use Tk;

# here we load the config (.gpligcrc) for the first time!
# later, we need to load it another time, because all defaults will be set
# while building the menus in FlightView()
load_config(1);

# check for Tk::JPEG module
# since Tk804.026 jpeg should be available
my $test_jpeg = eval{ require Tk::JPEG; };
if (defined $test_jpeg && $test_jpeg == 1) {
    $have_jpeg = 1;
    print "Tk::JPEG module found ;-) !\n" if ($config{'DEBUG'});
} else {
    print "Perl module: Tk::JPEG not available :,-( !\n";# if ($config{'DEBUG'});
}

my $test_png = eval{require Tk::PNG;};
if (defined $test_png && $test_png == 1) {
    $have_png = 1;
    print "PNG module found ;-) !\n" if ($config{'DEBUG'});
} else {
    print "Perl module: Tk::PNG not available :,-( !\n";# if ($config{'DEBUG'});
}

my $test_imager = eval{require Imager;};
if (defined $test_imager && $test_imager == 1) {
    $have_imager = 1;
    print "Imager module found ;-) !\n" if ($config{'DEBUG'});
} else {
    print "Perl module: Imager not available :,-( !\n";# if ($config{'DEBUG'});
}

# check for the Image::ExifTool module.
my $test_EXIF = eval{ require Image::ExifTool; };
if (defined $test_EXIF && $test_EXIF == 1) {
    $have_EXIF = 1;
    print "Image::ExifTool module found ;-) !\n" if ($config{'DEBUG'});
} else {
    print "Perl module: Image::ExifTool not available :,-( !\n";# if ($config{'DEBUG'});
}

# check environment vars on win
if ($^O eq "MSWin32") {
    if (defined $ENV{'GPLIGCHOME'}) {
        $libdir = $ENV{'GPLIGCHOME'}."/";
        $datadir = $ENV{'GPLIGCHOME'}."/";
    } else {
        $libdir = ".";
        $datadir = ".";
        print "GPLIGCHOME is not defined !!!\n";
    }

    # workaround for old buggy win32 tk module < 800.23 ?
    if (!defined $ENV{'HOME'}) {
        $ENV{'HOME'} = "C:\\";
    }
} else { # Unix-case: allow overriding libdir by GPLIGCHOME
    if (defined $ENV{'GPLIGCHOME'}) {
        print "GPLIGCHOME is defined: $ENV{'GPLIGCHOME'}\n";
        print "previous content of $libdir="."$libdir will be overridden.\n";
        $libdir = $ENV{'GPLIGCHOME'}."/";
        $datadir = $ENV{'GPLIGCHOME'}."/";
    }
}

# add libdir to infotext
$text .= "Library path: $libdir\n";
$text .= "Data path: $datadir\n";

use File::Spec;
my $curdir = File::Spec->curdir;

use File::Basename;
my $scriptpath = File::Basename::dirname($0)."/";

if ($config{'DEBUG'}) {
    print "$scriptpath $0\n";
    print "libdir: $libdir \n";
    print "datadir: $datadir \n";
    print "curdir: $curdir \n";
    print "scriptpath: $scriptpath \n";
}

# define $config{'working_directory'} the starting path for open,save,etc.
if ($^O eq "MSWin32") {
    if (defined $ENV{'GPLIGCHOME'}) {
        $config{'working_directory'} = $ENV{'GPLIGCHOME'};
    } else {
        $config{'working_directory'} = "C:\\";
    }
} else {
    if (defined $ENV{'HOME'}) {
        $config{'working_directory'} = $ENV{'HOME'};
    } else {
        $config{'working_directory'} = "/";
    }
}

unshift (@INC, $curdir, $scriptpath, $libdir);

# path seperator
my $ps;
if ($^O eq "MSWin32") { $ps =";";} else {$ps=":";}

$ENV{'PATH'} = "$ENV{'PATH'}$ps$curdir$ps$libdir$ps$scriptpath";

#catch some signals for controlled shutdown.
$SIG{'INT'} = \&gpligcexit;
$SIG{'QUIT'} = \&gpligcexit;
$SIG{'TERM'} = \&gpligcexit;

require GPLIGCfunctions;

if ($^O eq "MSWin32") {
    my ($osname, $edition) = Win32::GetOSName();
    $text .= "Windows version: $osname - $edition\n";
}

$text .= "Platform: ".$^O."  Perl: ".$]."  Tk: ".$Tk::VERSION."\n";

# additional modules version info ######################
if ($have_imager) {
    $text .= "Imager: ".$Imager::VERSION." ";
} else {
    $text .= "Imager: not inst. ";
}

if ($have_EXIF) {
    $text .= "Image::ExifTool: ".$Image::ExifTool::VERSION." ";
} else {
    $text .= "Image::ExifTool: not inst. ";
}

if ($have_jpeg) {
    $text .= "Tk::JPEG: ".$Tk::JPEG::VERSION." ";
} else {
    $text .= "Tk::JPEG: not inst. ";
}

if ($have_png) {
    $text .= "Tk::PNG: ".$Tk::PNG::VERSION."\n";
} else {
    $text .= "Tk::PNG: not inst.\n";
}
########################################################

# /B Option for windows (DOS) command "start" to start the new DOS-window minimized
# not available on all systems
my $start_parameter;

if ($^O eq "MSWin32") {
    $start_parameter = "/b";
    my ($osname, $edition) = Win32::GetOSName();

    # if $osname contains one of the list-entries, delete the /B option
    my @noB_OSlist = qw(Win95 Win98 WinME);  #these OS's dont know the /B option for start-command
    foreach (@noB_OSlist) {
        if ($osname =~ /$_/i) {
            $start_parameter = "";
        }
    }
}

# CREATE TMP-DIR
# Make a temporary directory, only if it does not exist already
# (|| will only get evaluated if it does _not_ exist yet.)
if ($^O ne "MSWin32") {
    $pfadftmp="/tmp/GPLIGC-$$/";
    -d $pfadftmp || mkdir $pfadftmp;
} else {
    $pfadftmp = $ENV{'TMP'}."\\GPLIGC-$$\\";
    -d $pfadftmp || mkdir $pfadftmp;
}

# ================ CREATE MAIN WINDOW ========================

FlightView();

# load configuration if available
# second time. after defaults were set in FlightView
load_config();

print "config loaded 2nd time\n" if ($config{'DEBUG'});

if ($config{'maps'} && !($have_imager && $have_png)) {
    Errorbox("The config{'maps'} is set, but imager and/or png modules are not available. I'll disable maps\n");
    $config{'maps'}=0;
}

$zl = $config{'maps_zoomlevel'};

# a timezone in config should be copied to gpi, before loading any files
$gpi{'timezone'}=$config{'timezone'};

# show new version welcome message
if ($config{'new_version_message_shown'} ne $version) {
    Errorbox($new_version_message);
    $config{'new_version_message_shown'} = $version;
}

if (!-d $config{'working_directory'}) {
    Errorbox("Working directory $config{'working_directory'} doesnt exits! Will be changed to $curdir\n");
    $config{'working_directory'} = $curdir;
}

if ($config{'skip_limit_minutes'} <= 0.017) {
    Errorbox("skip_limit_minutes is smaller than 0.017! That doesnt make sense! See manual. I'll set it to 0.1 now!");
    $config{'skip_limit_minutes'}=0.1;
}

# final check of configuration
check_config();

# check for a file on commandline and load it
# OR load "splash" image/text
if($#ARGV == 0 ){
    $file = $ARGV[0];
    oeffnen($file);
    updateFVW('i');
} else {
    FVWsetInfo();
}


### Main Event Loop
MainLoop;

###
###  Subroutines follow
###
###############################################################################

sub updateCyl {
    if (Exists($FlightView)) {
        if ($wpcyl_state == 1) {wpcyldraw(0); wpcyldraw(1);}
    }
}

sub updateZoom {
    if ($zoom == 1) { zoom(); zoom(); }
}

# update flight view window
sub updateFVW {

    #if (Exists($FlightView)) {
    my $initflag='u'; # update or initial   //mapplot braucht das flag irgendwie beim erstem mal 'i'
    if ($#_ != -1) { ## no parameters?
        $initflag='i' if (shift eq 'i') ;
	if (@introtext) {$canvas->delete(@introtext);}
    }

    # task and cylinder have to be erased first! (fix for bug #6)
    if ($task_state == 1) {taskdraw(0);}
    if ($wpcyl_state == 1) {wpcyldraw(0);}

    mapplot($dxp, $dyp, $minlat, $minlon, $initflag);
    trackplot($dxp, $dyp, $minlat, $minlon);
    marksdraw();
    baroplot();

    if ($wpcyl_state == 1) {wpcyldraw(1);}
    if ($task_state == 1) {taskdraw(1);}

    FVWausg();
}

sub updateCoor {
    FVWausg();
    if (Exists($WPPlotWindow)) {WPPlotUpdate();}
}

sub oeffnen {      ### open a File

    my $backup=$file;
    $file=shift;          ### Filename wird vom Fileselektor uebernommen (file=global)

    if ($file eq '') {$file=$backup;  return;}  ### Kein Filename -> zurueck

    my $length=length($file);
    my $test=substr($file, $length-3 ,3);   ### Fileendung in $test

    # check for igc-file (only file-ending...)
    if ($test ne 'igc' && $test ne 'IGC') {
        Errorbox("The file does not have igc or IGC ending. Trying to open anyway.");
    }

    if (! -r $file) {
        Errorbox("Can't open $file");$file='No file opened'; return;
    }

    #if (Exists($initpic)) {$initpic->destroy;}
    #reset alt-mode
    $altmode = "baro";
    $liftsexist=0;

    #reset some variables (this was at prog-start in 1.6)
    $xmin='';
    $xmax='';
    $ymin='';
    $ymax='';
    $zmin='0';
    $zmax='';
    $AKTWP=1;

    $freeWP=1;
    $offset=0;   #elevation offset, used as openGLIGCexplorer option... after using elevation calibration of FVW!
    $nr = 0 ;

    # reset additional info in gpi-data
    $gpi{'month'}="00";
    $gpi{'day'}="00";
    $gpi{'year'}="2009";
    $gpi{'timezone'}=$config{'timezone'};
    $gpi{'qnh'}=1013.25;  # (this is also set in the beginning)
    $gpi{'altitude_calibration_ref_press'}=0;
    $gpi{'altitude_calibration_type'}="none";
    $gpi{'altitude_calibration_point'}=0;
    $gpi{'altitude_calibration_point_alt'}=0;
    $gpi{'pilot'}="";
    $gpi{'copilot'}="";
    $gpi{'glider'}="";
    $gpi{'callsign'}="";
    $gpi{'airfield'}="";

    $gpi{'logger'}="";
    $gpi{'remarks'}="";
    $gpi{'remarks2'}="";
    $gpi{'remarks3'}="";

    open(FILE,"<$file") || return; ### wie fehlermeldung und return?????

    ($gpi{'year'}, $gpi{'month'}, $gpi{'day'}) = GPLIGCfunctions::guessdatefromfilename($file);

    ### Einsaugen der IGC-Datei von File
    @file=<FILE>; ###global! file fuer alle!!!
    close (FILE);
    $filestat='open'; ### Status : File geoeffnet

    # reset some variables
    $AKTWP=1;

    ###alle array werden definiert!
    @DECLON=();
    @DECLAT=();
    @DECTIME=();
    @BARO=();
    $BARO_max=-20000;
    $BARO_min=100000;
    @GPSALT=();
    $GPSALT_max=-20000;
    $GPSALT_min=100000;
    @TIME=();
    @LAT=();
    @LON=();
    @FIX=();
    my @FIXb=();
    @HEADER=();
    @TASK=();
    @ENL=();	#noise level
    @IAS=();	#indicated airspeed
    @TAS=();	#true airspeed
    @SIU=();	# satellites in used
    @FXA=();	# hor/vertical accuracy
    @VXA=();
    @RPM=();
    $IAS_max=-10000;
    $IAS_min=100000;
    $TAS_max=-10000;
    $TAS_min=100000;
    @ASPEED=();

    #	$ASPEED_max=-10000;
    #	$ASPEED_min=100000;
    @compVARIO=();
    @nonIVARIO=();
    $nonIVARIO_max=-100;
    $nonIVARIO_min=100;
    @nonIVARIOGPS=();
    $nonIVARIOGPS_max=-100;
    $nonIVARIOGPS_min=100;
    @IVARIO=();
    @IVARIOGPS=();
    @nonISPEED=();
    $nonISPEED_max=-200000;
    $nonISPEED_min=2000000;
    @TASK_info=();
    @WPIGCLAT=0;
    @WPIGCLON=0;
    @WPNAME=();
    @WPLAT=();
    @WPLON=();
    @WPRADFAC=(); # WP radius factor (for cylinder/barrel)

    # I-extensions to the B-record
    $IASEXISTS =  "no";
    $IASLENGTH =  0;
    $TASEXISTS =  "no";
    $TASLENGTH =  0;
    $ENLEXISTS =  "no";
    $ENLLENGTH =  0;
    $SIUEXISTS =  "no";
    $SIULENGTH =  0;
    $FXAEXISTS =  "no";
    $FXALENGTH =  0;
    $VXAEXISTS =  "no";
    $VXALENGTH =  0;
    $RPMEXISTS =  "no";
    $RPMLENGTH =  0;

    $TASKEXISTS = "no";

    ## Trigonometric distances from some reference point
    ## Skewed because we are on the surface of a sphere, but our
    ## flown distances are a lot << than that very sphere we are on...

    @KMY = ();
    @KMX = ();

    $busy=$FlightView->Busy;

    ### Schleife ueber alle Zeilen und Selektierung der Eintraege nach A H C B
    foreach my $zeile (@file) 	{

        if ($zeile=~/^A(.+)\S?/) {$FDR=$1;}
        if ($zeile=~/^H(.+)\S?/) {push(@HEADER , $1."\n");}

        if ($zeile=~/^C(.+)\S?/) {
            $TASKEXISTS="yes";
            push(@TASK , $1."\n");
        }

        if ($zeile=~/^B(.+)\S?/) {push(@FIXb , $1."\n");}

   # check for I_extensions to the Brecords (more here to be added SIU, FXA etc)
        if ($zeile=~/^I(.+)\S?/) {
            $izeile=$1;

            if ($izeile =~/(\d{2})(\d{2})ENL/) {
                $ENLSTART = $1 - 2;
                $ENLLENGTH = $2 -$1 +1;

                #$ENLEND = $2;
                $ENLEXISTS = "yes";
            }
            if ($izeile =~/(\d{2})(\d{2})RPM/) {
                $RPMSTART = $1 - 2;
                $RPMLENGTH = $2 -$1 +1;

                #$ENLEND = $2;
                $RPMEXISTS = "yes";
            }
            if ($izeile =~/(\d{2})(\d{2})IAS/) {
                $IASSTART = $1 - 2;
                $IASLENGTH = $2 - $1 +1;
                $IASEXISTS = "yes";
             }
            if ($izeile =~/(\d{2})(\d{2})SIU/) {
                $SIUSTART = $1 - 2;
                $SIULENGTH = $2 - $1 +1;
                $SIUEXISTS = "yes";
            }
            if ($izeile =~/(\d{2})(\d{2})FXA/) {
                $FXASTART = $1 - 2;
                $FXALENGTH = $2 - $1 +1;
                $FXAEXISTS = "yes";
            }
            if ($izeile =~/(\d{2})(\d{2})VXA/) {
                $VXASTART = $1 - 2;
                $VXALENGTH = $2 - $1 +1;
                $VXAEXISTS = "yes";
            }
            if ($izeile =~/(\d{2})(\d{2})TAS/) {
                $TASSTART = $1 - 2;
                $TASLENGTH = $2 -$1 +1;
                $TASEXISTS = "yes";
            }
        }
    }

    # grep all "safe" position fixes (if 24th character is V == unsafe)
    @FIX = grep { (substr($_,23,1)  ne 'V'); } @FIXb;

    # emergency Fallback, if no "A's" available
    if ($#FIX == -1) {
        Errorbox("No valid (A) position fixes found. Trying to use position fixes marked as invalid (V)");
        @FIX = @FIXb;
    }

  # no fixes avaiilable? no safe ones and no unsafe ones... sorry, nothing to do
    if ($#FIX == -1) {
        Errorbox("No position fixes found - invalid igc-file!");
        gpligcexit (1);
    }

    my $alterr=0;
    ### read  @BARO @GPSALT @TIME @LAT @LON from fixes-lines
    for (my $z=0; $z<= $#FIX; $z++) 	{
        my $skip=0;
        if ($config{'skip_check'} && $config{'skip_del_first_after'} && !$z<1) {
            if (GPLIGCfunctions::time2dec(substr($FIX[$z],0,6))-GPLIGCfunctions::time2dec(substr($FIX[$z-1],0,6)) > $config{'skip_limit_minutes'}/60 ) {$skip =1;}
        }

        if (! $skip) {
            my $alt = substr($FIX[$z], 24,5);
            my $gpsa = substr($FIX[$z], 29,5);
            if ($alt > 20000) {$alt = 0; $alterr=1;}
            if ($gpsa >20000) {$gpsa = 0; $alterr=1;}
            push(@BARO, $alt);
            push(@GPSALT, $gpsa);
            push(@TIME, substr($FIX[$z], 0,6));
            push(@LAT, substr($FIX[$z], 6, 8));
            push(@LON, substr($FIX[$z], 14, 9));

            if ($ENLEXISTS eq "yes") {
                push(@ENL, substr($FIX[$z], $ENLSTART, $ENLLENGTH));
            } else {
                push(@ENL, "0");
            }
            if ($RPMEXISTS eq "yes") {
                push(@RPM, substr($FIX[$z], $RPMSTART, $RPMLENGTH));
            } else {
                push(@RPM, "0");
            }

            if ($IASEXISTS eq "yes") {
                push(@IAS, substr($FIX[$z], $IASSTART, $IASLENGTH));
            } else {
                push(@IAS, "0");
            }

            if ($TASEXISTS eq "yes") {
                push(@TAS, substr($FIX[$z], $TASSTART, $TASLENGTH));
            } else {
                push(@TAS, "0");
            }

            if ($SIUEXISTS eq "yes") {
                push(@SIU, substr($FIX[$z], $SIUSTART, $SIULENGTH));
            } else {
                push(@SIU, "0");
            }

            if ($FXAEXISTS eq "yes") {
                push(@FXA, substr($FIX[$z], $FXASTART, $FXALENGTH));
            } else {
                push(@FXA, "0");
            }
            if ($VXAEXISTS eq "yes") {
                push(@VXA, substr($FIX[$z], $VXASTART, $VXALENGTH));
            } else {
                push(@VXA, "0");
            }

        }
    }

    if ($alterr) {
        Errorbox("Very weird altitudes found! (>20.000m) They are set to 0. Check your igc-file.");
    }

    # convert time to decimal
    foreach (@TIME)	{
        $tmptime=GPLIGCfunctions::time2dec($_);
        push(@DECTIME, $tmptime);
    }

    # convert coordinates to decimal degrees
    foreach (@LAT)	{
        push(@DECLAT, GPLIGCfunctions::igc2dec($_));
    }

    foreach (@LON) 	{
        push(@DECLON, GPLIGCfunctions::igc2dec($_));
    }

    # Find reference point (center between max and min lat/lon)
    my ($maxlat, $minlat)=GPLIGCfunctions::MaxKoor(\@DECLAT);
    my ($maxlon, $minlon)=GPLIGCfunctions::MaxKoor(\@DECLON);
    $centrelon = ($minlon + $maxlon)/2;
    $centrelat = ($minlat + $maxlat)/2;


    print "OEFFNEN: init maxlat minlat maxlon minlon: $maxlat  $minlat  $maxlon  $minlon\n" if ($config{'DEBUG'});

    # KMY and KMX are km distances in X ynd Y from refernece point

    for ($i=0; $i<=$#DECLAT; $i++){
        my $dist=GPLIGCfunctions::dist($centrelat,$DECLON[$i],$DECLAT[$i],$DECLON[$i]);
        if($centrelat>$DECLAT[$i]){
            $dist = -$dist;
        }

        #print "$i  $dist\n";
        push(@KMY, $dist);
    }

    for ($i=0; $i<=$#DECLON; $i++){
        my $dist=GPLIGCfunctions::dist($DECLAT[$i],$centrelon,$DECLAT[$i],$DECLON[$i]);
        if($centrelon>$DECLON[$i]){
            $dist = -$dist;
        }
        push(@KMX, $dist);
    }

# average speed, vario, gpsvario (averaged by $config{'integrate_over'} position fixes)
    for ($i=0; $i<=$#DECLAT; $i++){
        if ($i >= $config{'integrate_over'}) {
            my $dist=GPLIGCfunctions::dist($DECLAT[$i-$config{'integrate_over'}],$DECLON[$i-$config{'integrate_over'}],$DECLAT[$i],$DECLON[$i]);
            push(@ASPEED, ($dist/($DECTIME[$i]-$DECTIME[$i-$config{'integrate_over'}])));
            push(@IVARIO, (($BARO[$i]-$BARO[$i-$config{'integrate_over'}])/(($DECTIME[$i]-$DECTIME[$i-$config{'integrate_over'}])*3600)) );
            push(@IVARIOGPS, (($GPSALT[$i]-$GPSALT[$i-$config{'integrate_over'}])/(($DECTIME[$i]-$DECTIME[$i-$config{'integrate_over'}])*3600))  );

        } else {
            push(@ASPEED, 0);
            push(@IVARIO, 0);
            push(@IVARIOGPS, 0);
        }
    }

    # non averaged speed, vario. gpsvario TE vario(compVARIO)
    for ($i=0; $i<=$#DECLAT; $i++)	{
        if ($i >= 1) {# && $i <$#DECLAT) {
            my $dist=GPLIGCfunctions::dist($DECLAT[$i-1],$DECLON[$i-1],$DECLAT[$i],$DECLON[$i]);

            my $delta_time=$DECTIME[$i]-$DECTIME[$i-1];

            #my $delta_time_next=$DECTIME[$i+1]-$DECTIME[$i];
            # WORKAROUND to avoid div by zero, if we have two identic B-records
            if ($delta_time == 0) {$delta_time=0.00000001;Errorbox("Identical B records! check IGC file at ".GPLIGCfunctions::dec2time($DECTIME[$i])) }

            #if ($delta_time_next == 0) {$delta_time_next=0.00000001;}

            # check for time reversals!
            # exception is 24:00->0:00 with a small window
            # might be problematic for large interval files... :-(

            my $trc=0; #time reversal count
            my $firsttrcat=999; #first trc
            my $nn; #n-1
            if ($DECTIME[$i] < $DECTIME[$i-1] && !(      $DECTIME[$i]<=0.2
                    && $DECTIME[$i]>=0
                    && $DECTIME[$i-1]<=24
                    && $DECTIME[$i-1]>=23.8   )) {

    # ??? why this? $DECTIME[$i] = $DECTIME[$i-1] + 0.000014; # a half a second!
                print " $DECTIME[$i]  $DECTIME[$i-1] \n" if ($config{'DEBUG'});

                if ($firsttrcat == 999) {
                    $firsttrcat = $DECTIME[$i];
                    $nn = $DECTIME[$i-1];
                }
                $trc++;
            }
            if ($trc > 0) {
                Errorbox("$trc time reversal(s) found. between ".GPLIGCfunctions::dec2time($nn)."-".GPLIGCfunctions::dec2time($firsttrcat)."! Check your IGC file!");
            }

           # filter abnormal values....
           #my $nonispeed = (($dist/$delta_time)+($distn/$delta_time_next))/2.0;
            my $nonispeed = ($dist/$delta_time);
            if ($nonispeed < 1200) {
                push(@nonISPEED, $nonispeed);
            } else {
                push(@nonISPEED, 0);
            }

#			my $nonivario = ((($BARO[$i]-$BARO[$i-1])/($delta_time*3600)) + (($BARO[$i+1]-$BARO[$i])/($delta_time_next*3600)))/2.0 ;
            my $nonivario = ($BARO[$i]-$BARO[$i-1])/($delta_time*3600);
            if (abs($nonivario) < 60) {
                push(@nonIVARIO,$nonivario);
            } else {
                push(@nonIVARIO,0);
            }

            if ($IASEXISTS eq "yes") {
                my $compvario = ( ($BARO[$i]-$BARO[$i-1]) -
                      ( ( (($IAS[$i-1]/3.6)**2) - ($IAS[$i]/3.6)**2) / 19.62) ) / ($delta_time*3600);
                push(@compVARIO,$compvario);

	    } elsif ($TASEXISTS eq "yes") {  ## XXX is this necessary for TAS?
                my $compvario = ( ($BARO[$i]-$BARO[$i-1]) -
                      ( ( (($TAS[$i-1]/3.6)**2) - ($TAS[$i]/3.6)**2) / 19.62) ) / ($delta_time*3600);
                push(@compVARIO,$compvario);

            } elsif ($config{'te_vario_fallback'}) { # ist das hier die richtige speed?
                my $compvario = ( ($BARO[$i]-$BARO[$i-1]) -
                      ( ( (($nonISPEED[$i-1]/3.6)**2) - ($nonISPEED[$i]/3.6)**2) / 19.62) ) / ($delta_time*3600);
                push(@compVARIO,$compvario);
            }
            else {push(@compVARIO,0);}

#my $gpsvario = ((($GPSALT[$i]-$GPSALT[$i-1])/($delta_time*3600))+ (($GPSALT[$i+1]-$GPSALT[$i])/($delta_time_next*3600)))/2.0 ;
            my $gpsvario = ($GPSALT[$i]-$GPSALT[$i-1])/($delta_time*3600);
            if (abs($gpsvario) < 60) {
                push(@nonIVARIOGPS,$gpsvario);
            } else {
                push(@nonIVARIOGPS,0);
            }

        } else { # this can be only calculated from the second data record. for the first record they will be set to zero
            push(@nonISPEED, 0);
            push(@nonIVARIO, 0);
            push(@nonIVARIOGPS, 0);
            if ($IASEXISTS eq "yes" || $TASEXISTS eq "yes" || $config{'te_vario_fallback'} == 1) {
                push(@compVARIO,0);
            }
        }
    }

    # warning
    if (($IASEXISTS ne "yes" && $TASEXISTS ne "yes") && $config{'te_vario_fallback'} == 1 && $config{'te_warning'})
    {Errorbox("total energy compensation was calculated from gps groundspeed!\nTo turn off this warning set te_warning = 0.\nAlso see te_vario_fallback");}

    # Maximum/minimum height:
    ($maxbaro, $minbaro)=GPLIGCfunctions::MaxKoor(\@BARO);
    ($maxgpsalt, $mingpsalt)=GPLIGCfunctions::MaxKoor(\@GPSALT);

    # write tmp-files
    tmpfileout();

    sub tmpfileout {

        open (BARO,">${pfadftmp}baro.dat");
        open (GPSALT,">${pfadftmp}gpsalt.dat");
        open (PRESS,">${pfadftmp}press.dat");
        open (SPACE,">${pfadftmp}3d.dat");
        open (LATPROJ,">${pfadftmp}latproj.dat");
        open (LONPROJ,">${pfadftmp}lonproj.dat");
        open (DELTAH,">${pfadftmp}deltah.dat");
        open (VARIO,">${pfadftmp}vario.dat");
        open (GPSVARIO,">${pfadftmp}gpsvario.dat");
        open (SPEED,">${pfadftmp}speed.dat");
        open (LONLAT,">${pfadftmp}lonlat.dat");
        open (KM,">${pfadftmp}km.dat");
        open (ENL, ">${pfadftmp}enl.dat");
        open (IAS, ">${pfadftmp}ias.dat");
        open (TAS, ">${pfadftmp}tas.dat");
        open (SAT, ">${pfadftmp}sat.dat");

     # ----------------- LOOP over all data-records (fixes) --------------------

        # writing tmp-files
        for (my $i=0; $i<=$#DECTIME; $i++)	{  # DECTIME nicht FIX!

            # processing of decimal time.....
            # check if we pass the 24:00
            if ($i > 0) {   # not in first cycle

                # if time goes backwards  24:00 -> 0:00
                if ($DECTIME[$i] < $DECTIME[$i-1]) {

                    print "2400 -> 0000 detected, altering DECTIME...\n" if ($config{'DEBUG'});
                    print " $#DECTIME, dect,,,,,  $#FIX fix...\n" if ($config{'DEBUG'});

                    # alter all time-points after this one... + 24
                    for (my $z=$i; $z<=$#DECTIME; $z++) {
                        $DECTIME[$z] = $DECTIME[$z] + 24;
                    }
                }
            }

            print KM $KMX[$i]," ",$KMY[$i]," ",$BARO[$i],"\n";
            print LONLAT $i," ",$LON[$i]," ",$LAT[$i],"\n";
            print BARO $DECTIME[$i]," ",$BARO[$i],"\n";
            print PRESS $DECTIME[$i]," ",GPLIGCfunctions::pressure($BARO[$i],$gpi{'qnh'}),"\n";
            if ($VXAEXISTS eq "no") {
                print GPSALT $DECTIME[$i]," ",$GPSALT[$i],"\n";
            } else {
                print GPSALT $DECTIME[$i]," ",$GPSALT[$i]," ", $VXA[$i],"\n";
            }
            print ENL $DECTIME[$i], " ", $ENL[$i], " ", $RPM[$i], "\n";
            print IAS $DECTIME[$i], " ", $IAS[$i],"\n";
            print TAS $DECTIME[$i], " ", $TAS[$i],"\n";
            print SPACE $DECLON[$i]," ",$DECLAT[$i]," ",$BARO[$i]," ",$GPSALT[$i],"\n";
            print LATPROJ $DECLAT[$i]," ",$BARO[$i]," ",$GPSALT[$i],"\n";
            print LONPROJ $DECLON[$i]," ",$BARO[$i]," ",$GPSALT[$i],"\n";
            print DELTAH $DECTIME[$i]," ",$BARO[$i]-$GPSALT[$i],"\n";

            # averaged vario, speed, gps-vario
            print VARIO $DECTIME[$i]," ",$IVARIO[$i],"\n";
            print GPSVARIO $DECTIME[$i]," ",$IVARIOGPS[$i],"\n";
            print SPEED $DECTIME[$i]," ",$ASPEED[$i],"\n";

            print SAT $DECTIME[$i]," ",$SIU[$i]," ",$FXA[$i]," ", $VXA[$i],"\n";

            # getting extrema of some values;

            $BARO_max = $BARO[$i] if ($BARO[$i] > $BARO_max);
            $BARO_min = $BARO[$i] if ($BARO[$i] < $BARO_min);
            $GPSALT_max = $GPSALT[$i] if ($GPSALT[$i] > $GPSALT_max);
            $GPSALT_min = $GPSALT[$i] if ($GPSALT[$i] < $GPSALT_min);

            $nonISPEED_max = $nonISPEED[$i] if ($nonISPEED[$i] > $nonISPEED_max);
            $nonISPEED_min = $nonISPEED[$i] if ($nonISPEED[$i] < $nonISPEED_min);

            $IAS_max = $IAS[$i] if ($IAS[$i] > $IAS_max);
            $IAS_min = $IAS[$i] if ($IAS[$i] < $IAS_min);

            $TAS_max = $TAS[$i] if ($TAS[$i] > $TAS_max);
            $TAS_min = $TAS[$i] if ($TAS[$i] < $TAS_min);

            $nonIVARIO_max = $nonIVARIO[$i] if ($nonIVARIO[$i] > $nonIVARIO_max);
            $nonIVARIO_min = $nonIVARIO[$i] if ($nonIVARIO[$i] < $nonIVARIO_min);

            $nonIVARIOGPS_max = $nonIVARIOGPS[$i] if ($nonIVARIOGPS[$i] > $nonIVARIOGPS_max);
            $nonIVARIOGPS_min = $nonIVARIOGPS[$i] if ($nonIVARIOGPS[$i] < $nonIVARIOGPS_min);

        }

        #rounding vario-extrema
        $nonIVARIO_max = sprintf("%.4f",$nonIVARIO_max);
        $nonIVARIO_min = sprintf("%.4f",$nonIVARIO_min);
        $nonIVARIOGPS_max = sprintf("%.4f",$nonIVARIOGPS_max);
        $nonIVARIOGPS_min = sprintf("%.4f",$nonIVARIOGPS_min);

        # closing files
        close (BARO);
        close (GPSALT);
        close (SPACE);
        close (LATPROJ);
        close (LONPROJ);
        close (DELTAH);
        close (VARIO);
        close (SPEED);
        close (LONLAT);
        close (ENL);
        close (IAS);
        close (TAS);
        close (KM);
        close (SAT);

    }

    if ($config{'DEBUG'}) {

	print "Debugging in Oeffnen: max and min values\n";
        print	"baro	$BARO_max \n";#= $BARO[$i] if ($BARO[$i] > $BARO_max);
        print	"	$BARO_min \n";#= $BARO[$i] if ($BARO[$i] < $BARO_min);
        print	"gps	$GPSALT_max \n";#= $GPSALT[$i] if ($GPSALT[$i] > $GPSALT_max);
        print	"	$GPSALT_min \n";#= $GPSALT[$i] if ($GPSALT[$i] < $GPSALT_min);
        print	"nISp	$nonISPEED_max \n";#= $nonISPEED[$i] if ($nonISPEED[$i] > $nonISPEED_max);
        print	"	$nonISPEED_min \n";#= $nonISPEED[$i] if ($nonISPEED[$i] < $nonISPEED_min);
        print	"IAS	$IAS_max \n";#= $IAS[$i] if ($IAS[$i] > $IAS_max);
        print	"	$IAS_min \n";#= $IAS[$i] if ($IAS[$i] < $IAS_min);
        print	"TAS	$TAS_max \n";#= $TAS[$i] if ($TAS[$i] > $TAS_max);
        print	"	$TAS_min \n";#= $TAS[$i] if ($TAS[$i] < $TAS_min);
        print	"nIV	$nonIVARIO_max \n";#= $nonIVARIO[$i] if ($nonIVARIO[$i] > $nonIVARIO_max);
        print	"	$nonIVARIO_min \n";#= $nonIVARIO[$i] if ($nonIVARIO[$i] < $nonIVARIO_min);
        print	"nIVgps	$nonIVARIOGPS_max \n";#= $nonIVARIOGPS[$i] if ($nonIVARIOGPS[$i] > $nonIVARIOGPS_max);
        print	"	$nonIVARIOGPS_min \n";#= $nonIVARIOGPS[$i] if ($nonIVARIOGPS[$i] < $nonIVARIOGPS_min);

    }

    # Generating HISTOGRAMS

    # we need the decimal flighttime!
    my $starttimeindex=GPLIGCfunctions::takeoff_detect(\@nonISPEED, 's');
    my $landtimeindex=GPLIGCfunctions::takeoff_detect(\@nonISPEED, 'l');
    my $decflighttime=$DECTIME[$landtimeindex]-$DECTIME[$starttimeindex];

    # histogram intervals for vario
    my $histo_intervall_vario = $config{'vario_histo_intervall'};#0.2;

    # histogram intervals and limits for speed
    my $histo_intervall = $config{'speed_histo_intervall'};#2.5;

    ### GROUND SPEED HISTOGRAM
    # initialize arrays for speed histogram
    my @speedBEREICH=();
    my @speedHISTO=();
    my $i = 0;
    for (my $bereich=$nonISPEED_min; $bereich <= $nonISPEED_max; $bereich+=$histo_intervall) {
        push(@speedHISTO,0);
        push(@speedBEREICH,$bereich);
    }

    # summarize the intervals
    for (my $z=$starttimeindex ; $z < $landtimeindex; $z++) {
        my $index = int(($nonISPEED[$z] - $nonISPEED_min ) / $histo_intervall);

        #print "$index \n";
        if ($index >= 0 && $index < (($nonISPEED_max-$nonISPEED_min)/$histo_intervall)) {

            #print "if $index\n";
            $speedHISTO[$index]+=  $DECTIME[$z+1]-$DECTIME[$z];
        }
    }

    #output
    open (SHISTO,">${pfadftmp}shisto.dat");

    #output in % of flighttime!
    for (my $z=0; $z < $#speedHISTO; $z++) {

        #print "$z   $#speedHISTO  \n";
        print SHISTO $speedBEREICH[$z]+($histo_intervall/2)."   ".(($speedHISTO[$z]/$decflighttime)*100)."\n";
    }
    close (SHISTO);

    ### AIRSPEED HISTOGRAM
    # initialize arrays
    @speedBEREICH=();
    @speedHISTO=();
    $i = 0;

    # initialize the histo-arrays
    for (my $bereich=$IAS_min; $bereich <= $IAS_max; $bereich+=$histo_intervall) {
        push(@speedHISTO,0);
        push(@speedBEREICH,$bereich);
    }

    # summieren der intervalle
    for (my $z=$starttimeindex ; $z < $landtimeindex; $z++) {
        my $index = int(($IAS[$z] - $IAS_min ) / $histo_intervall);

        #print "$index \n";
        if ($index >= 0 && $index < (($IAS_max-$IAS_min)/$histo_intervall)) {

            #print "if $index\n";
            $speedHISTO[$index]+=  $DECTIME[$z+1]-$DECTIME[$z];
        }
    }

    #output
    open (ASHISTO,">${pfadftmp}iashisto.dat");

    #output in % of flighttime!
    for (my $z=0; $z < $#speedHISTO; $z++) {

        #print "$z   $#speedHISTO  \n";
        print ASHISTO $speedBEREICH[$z]+($histo_intervall/2)."   ".(($speedHISTO[$z]/$decflighttime)*100)."\n";
    }
    close (ASHISTO);

    ### AIRSPEED HISTOGRAM
    # initialize arrays
    @speedBEREICH=();
    @speedHISTO=();
    $i = 0;

    # initialize the histo-arrays
    for (my $bereich=$TAS_min; $bereich <= $TAS_max; $bereich+=$histo_intervall) {
        push(@speedHISTO,0);
        push(@speedBEREICH,$bereich);
    }

    # summieren der intervalle
    for (my $z=$starttimeindex ; $z < $landtimeindex; $z++) {
        my $index = int(($TAS[$z] - $TAS_min ) / $histo_intervall);

        #print "$index \n";
        if ($index >= 0 && $index < (($TAS_max-$TAS_min)/$histo_intervall)) {

            #print "if $index\n";
            $speedHISTO[$index]+=  $DECTIME[$z+1]-$DECTIME[$z];
        }
    }

    #output
    open (ASHISTO,">${pfadftmp}tashisto.dat");

    #output in % of flighttime!
    for (my $z=0; $z < $#speedHISTO; $z++) {

        #print "$z   $#speedHISTO  \n";
        print ASHISTO $speedBEREICH[$z]+($histo_intervall/2)."   ".(($speedHISTO[$z]/$decflighttime)*100)."\n";
    }
    close (ASHISTO);

    ### VARIO HISTO BARO

    my @varioBEREICH=();
    my @varioHISTO=();
    $i = 0;

    # initialize the histo-arrays
    for (my $bereich=$nonIVARIO_min; $bereich <= $nonIVARIO_max; $bereich+=$histo_intervall_vario) {
        push(@varioHISTO,0);
        push(@varioBEREICH,$bereich);
    }


    # summieren der intervalle
    for (my $z=$starttimeindex ; $z < $landtimeindex; $z++) {
        my $index = int(($nonIVARIO[$z] - $nonIVARIO_min ) / $histo_intervall_vario);

        #print "$index \n";
        if ($index >= 0 && $index < (($nonIVARIO_max-$nonIVARIO_min)/$histo_intervall_vario)) {

            #print "if $index\n";
            $varioHISTO[$index]+=  $DECTIME[$z+1]-$DECTIME[$z];
        }
    }


    #output
    open (VHISTO,">${pfadftmp}vhisto.dat");

    #output in % of flighttime!
    for (my $z=0; $z < $#varioHISTO; $z++) {
        print VHISTO $varioBEREICH[$z]+($histo_intervall_vario/2)."   ".(($varioHISTO[$z]/$decflighttime)*100)."\n";
    }
    close (VHISTO);

    ### VARIO HISTO GPS

    @varioBEREICH=();
    @varioHISTO=();
    $i = 0;

    # initialize the histo-arrays
    for (my $bereich=$nonIVARIOGPS_min; $bereich <= $nonIVARIOGPS_max; $bereich+=$histo_intervall_vario) {
        push(@varioHISTO,0);
        push(@varioBEREICH,$bereich);

        #print "$bereich ..\n";
    }

    # summieren der intervalle

    for (my $z=$starttimeindex ; $z < $landtimeindex; $z++) {
        my $index = int((($nonIVARIOGPS[$z] - $nonIVARIOGPS_min ) / $histo_intervall_vario)+.5);

        #print "$nonIVARIOGPS[$z] \n";

        if ($index >= 0 && $index < (($nonIVARIOGPS_max-$nonIVARIOGPS_min)/$histo_intervall_vario)) {

            #print "if $index\n";
            $varioHISTO[$index]+=  $DECTIME[$z+1]-$DECTIME[$z];
        }
    }

    #output
    open (VHISTOGPS,">${pfadftmp}vhistogps.dat");

    #output in % of flighttime!
    for (my $z=0; $z < $#varioHISTO; $z++) {
        print VHISTOGPS $varioBEREICH[$z]+($histo_intervall_vario/2)."   ".(($varioHISTO[$z]/$decflighttime)*100)."\n";
    }
    close (VHISTOGPS);

    ### BARO HISTO

    @baroBEREICH=();
    @baroHISTO=();
    $i = 0;

    # initialize the histo-arrays
    for (my $bereich=$BARO_min; $bereich <= $BARO_max; $bereich+=$config{'baro_histo_intervall'}) {
        push(@baroHISTO,0);
        push(@baroBEREICH,$bereich);
    }

    # summieren der intervalle
    for (my $z=$starttimeindex ; $z < $landtimeindex; $z++) {
        my $index = int(($BARO[$z] - $BARO_min ) / $config{'baro_histo_intervall'});

        #print "$index \n";
        if ($index >= 0 && $index < (($BARO_max-$BARO_min)/$config{'baro_histo_intervall'})) {

            #print "if $index\n";
            $baroHISTO[$index]+=  $DECTIME[$z+1]-$DECTIME[$z];
        }
    }


    #output
    open (BAROHISTO,">${pfadftmp}barohisto.dat");

    #output in % of flighttime!
    for (my $z=0; $z < $#baroHISTO; $z++) {
        print BAROHISTO $baroBEREICH[$z]+($config{'baro_histo_intervall'}/2)."   ".(($baroHISTO[$z]/$decflighttime)*100)."\n";
    }
    close (BAROHISTO);

    ### GPS HISTO

    @baroBEREICH=();
    @baroHISTO=();
    $i = 0;

    # initialize the histo-arrays
    for (my $bereich=$GPSALT_min; $bereich <= $GPSALT_max; $bereich+=$config{'baro_histo_intervall'}) {
        push(@baroHISTO,0);
        push(@baroBEREICH,$bereich);
    }

    # summieren der intervalle
    for (my $z=$starttimeindex ; $z < $landtimeindex; $z++) {
        my $index = int(($GPSALT[$z] - $GPSALT_min ) / $config{'baro_histo_intervall'});

        #print "$index \n";
        if ($index >= 0 && $index < (($GPSALT_max-$GPSALT_min)/$config{'baro_histo_intervall'})) {

            #print "if $index\n";
            $baroHISTO[$index]+=  $DECTIME[$z+1]-$DECTIME[$z];
        }
    }


    #output
    open (BAROHISTO,">${pfadftmp}gpshisto.dat");

    #output in % of flighttime!
    for (my $z=0; $z < $#baroHISTO; $z++) {
        print BAROHISTO $baroBEREICH[$z]+($config{'baro_histo_intervall'}/2)."   ".(($baroHISTO[$z]/$decflighttime)*100)."\n";
    }
    close (BAROHISTO);

    ##################
    # This is a work-around for files without pressure altitude...
    # hell knows how that can happen, but I have seen such.
    $dybaro = ($maxbaro - $minbaro);

    if($dybaro == 0){ #some sick files only have GPS altitude...
        Errorbox("Work-around enabled! NO PRESSURE ALTITUDE FOUND IN FILE! USING GPS ALTITUDE");
        $minbaro = $mingpsalt;
        $maxbaro = $maxgpsalt;
        @BARO = @GPSALT;
        @nonIVARIO = @nonIVARIOGPS;
        @IVARIO = @IVARIOGPS;
        $altmode = "gps";
    }
    #################

    # evaluation of @TASK
    my $z=0;
    $MAXWP=0;

    my $j=1;

    # this is the new and better task scan-function
    foreach (@TASK) {
        if (substr($_,7,1) ne 'N' && substr($_,7,1) ne 'S') {

            # $_ is a task-definition !

            $TASK_ID=substr($_,18,4);
            my $zwisch_date=GPLIGCfunctions::time2human(substr($_,0,6),'d');
            my $zwisch_zeit=GPLIGCfunctions::time2human(substr($_,6,6),'t');
            $TASK_TIME=$zwisch_date."  ".$zwisch_zeit;
            $TASK_DATE=GPLIGCfunctions::time2human(substr($_,12,6),'d');
            $TASK_WPs=substr($_,22,2);

            #$MAXWP=$MAXWP+($TASK_WPs+4); # + start, abflug, finish, landing

            push(@TASK_info, "------------");
            push(@TASK_info, "Task ID: $TASK_ID");

            push(@TASK_info, "Task set time: $TASK_TIME");
            push(@TASK_info, "Task date: $TASK_DATE");
            push(@TASK_info, "#WPs : $TASK_WPs");

        } else {

            #only if different from last one (avoid the doubled entries)
            if ( (substr($_,0,8) ne $WPIGCLAT[$j-1] && substr($_,8,9) ne $WPIGCLON[$j-1]) &&
                   !( substr($_,0,7) eq "0000000" && substr($_,8,8) eq "00000000" )  # ignore 0/0 coordinates (invalid WPs) Bug #4
               ) {

                $WPIGCLAT[$j]=substr($_,0,8);
                $WPIGCLON[$j]=substr($_,8,9);
                $WPNAME[$j]=substr($_,17);
                $WPRADFAC[$j]=1.0;
                chop($WPNAME[$j]);  #cut CR
                chop($WPNAME[$j]);  #cut LF

                my $formlat=GPLIGCfunctions::coorconvert(GPLIGCfunctions::igc2dec($WPIGCLAT[$j]),'lat','igch');
                my $formlon=GPLIGCfunctions::coorconvert(GPLIGCfunctions::igc2dec($WPIGCLON[$j]),'lon','igch');

               #push(@TASK_info, "$WPIGCLAT[$j]   $WPIGCLON[$j]   $WPNAME[$j]");
                push(@TASK_info, "$formlat   $formlon   $WPNAME[$j]");
                $j++;
                $MAXWP++;
            } else { # if doubled, put in TASK_info trotzdem
                my $wpname=substr($_,17);
                my $wplat=substr($_,0,8);
                my $wplon=substr($_,8,9);

                chop($wpname);  #cut CR
                chop($wpname);  #cut LF
                my $formlat=GPLIGCfunctions::coorconvert(GPLIGCfunctions::igc2dec($wplat),'lat','igch');
                my $formlon=GPLIGCfunctions::coorconvert(GPLIGCfunctions::igc2dec($wplon),'lon','igch');
                push(@TASK_info, "$formlat   $formlon   $wpname");
            }
        }
    }

    #if there is no waypoint/task in the igc-file, we set a single WP in the centre
    if ($TASKEXISTS eq "no" || $j == 1) {  #j==1 -> no valid wp read!
        $MAXWP =1;

        # define in igc format, conversion is done latr (for all WPs)
        $WPIGCLAT[1]=GPLIGCfunctions::coorconvert($centrelat, "lat", "igc");
        $WPIGCLON[1]=GPLIGCfunctions::coorconvert($centrelon, "lon", "igc");
        $WPNAME[1]="Centre";
        $WPRADFAC[1]=1.0;
    }

    # convert all wp's to decimal format!
    for ($i=1; $i<=$#WPIGCLAT; $i++){
        $WPLAT[$i]=GPLIGCfunctions::igc2dec($WPIGCLAT[$i]);
        $WPLON[$i]=GPLIGCfunctions::igc2dec($WPIGCLON[$i]);
    }

    # set start and finish time (for task_speed) initially to takeoff and landing time
    # also the unpowered flight window
    $task_finish_time_index = GPLIGCfunctions::takeoff_detect(\@nonISPEED,'l');
    $task_start_time_index = GPLIGCfunctions::releaseDetect(\@BARO,GPLIGCfunctions::takeoff_detect(\@nonISPEED,'s'));

    foreach (@HEADER) {
        # i have to chop off trailing garbage first
        $_ =~  s/\s+$// ;
        if (/^FPLT[\w\d\s]+:(.+)$/) {$gpi{'pilot'} = $1}
        if (/^FGTY[\w\d\s]+:(.+)$/) {$gpi{'glider'} = $1}
        if (/^FGID[\w\d\s]+:(.+)$/) {$gpi{'callsign'} = $1}
        if ($gpi{'year'} == 0 && $gpi{'month'} == 0 && $gpi{'day'} == 0) {
            if (/^FDTE(\d{6})/) {$gpi{'day'} = substr($1,0,2); $gpi{'month'}=substr($1,2,2); $gpi{'year'}=substr($1,4,2);
                if ($gpi{'year'} > 90) {$gpi{'year'} +=1900} else {$gpi{'year'} +=2000}
            }
        }
    }

    $stat_start =0;
    $stat_end= $#BARO;

    # check for gpi file and load if present!
    # if not

    my $gpifile = substr($file, 0, -3)."gpi";

    if (-r $gpifile) {load_gpi();}
      elsif ($config{'open_additional_info'} == 1) {
        gpiinput();
      }

    if ($gpi{'altitude_calibration_type'} eq "baro") {elevationcalibration("baro",$gpi{'altitude_calibration_point_alt'},$gpi{'altitude_calibration_point'},1);}
    if ($gpi{'altitude_calibration_type'} eq "constant") {elevationcalibration("constant",$gpi{'altitude_calibration_point_alt'},$gpi{'altitude_calibration_point'},1);}
    if ($gpi{'altitude_calibration_type'} eq "refpress") {qnhcalibration($gpi{'qnh'},$gpi{'altitude_calibration_ref_press'},1);}

    unzoomed();

    if ($config{'photos'}) {
        print "Starting to process Photo-stuff.... .... \n" if ($config{'DEBUG'});
        checkPhotos();
        processPhotos();
        print "..... processing Photo-stuff finished. \n" if ($config{'DEBUG'});
    }

    my($ofilename, $odirectories, $osuffix) = File::Basename::fileparse($file);
    $FlightView->configure(-title=>"$ofilename");#.$osuffix"); #<-- osuffix empty
    $busy=$FlightView->Unbusy;

    print "sub oeffnen fertig\n" if ($config{'DEBUG'});
}
# oeffnen
###############################################################################

# make plots with gnuplot
sub gnuplot {
    if ($filestat eq 'closed') {
        $FlightView->bell;
        return;
    }

    #check for gnuplot!
    if ($^O ne "MSWin32") {
        if (system("echo 'q' | gnuplot")) {
            Errorbox("Can't execute gnuplot. Please check your gnuplot installation");
            return;
        }
    }

    my $mode = shift;  #(2d or 3d)

    my $bereich='['.$xmin.':'.$xmax.']['.$ymin.':'.$ymax.']['.$zmin.':'.$zmax.']';

    # Linux/Unix Gnuplot 3.x: in this case we'll write into the pipe to gnuplot
    if ( $^O ne "MSWin32"  &&  $config{'gnuplot_major_version'} != 4 ) {
        open (BPIPE, "| gnuplot -persist");    ### open GNUPLOT write-Pipe
        print "using old gnuplot 3.x: writing to pipe | gnuplot -persist\n" if ($config{'DEBUG'});

    } else { # other (Win32 and/or gnuplot 4.x): we'll write commands into plot.gpl

        unlink ("${pfadftmp}plot.gpl");
        open (BPIPE, ">${pfadftmp}plot.gpl"); # command file for gnuplot
        print "using new gnuplot 4.x : writing into plot.gpl\n" if ($config{'DEBUG'} && $config{'gnuplot_major_version'} == 4);
        print "using windows gnuplot: $config{'gnuplot_win_exec'} \n" if ($config{'DEBUG'} && $^O eq "MSWin32") ;
    }

    my $w32_persist;
    my $linuxquiet = '';
    if ($^O ne "MSWin32") {$linuxquiet =' 2>/dev/null >&2';}

    if ( ($config{'gnuplot_terminal'} ne 'x11') && ($config{'gnuplot_terminal'} ne 'qt')){ # plot goes to file
        if ($savname eq 'no'){$savname=save();}
        if ($savname eq 'no'){close BPIPE;return;}
        if ($savname ne 'no'){
                 print BPIPE "set term $config{'gnuplot_terminal'}\n";
                 print BPIPE "set output "."\'".$savname."\'"."\n";
        }
        $w32_persist = "";
    } else {
        # plot goes to screen
        # dont set term on windows (we're using the default on win32)
        print BPIPE "set term $config{'gnuplot_terminal'}\n" if ($^O ne "MSWin32");
        $w32_persist = "-";
    }

    $busy=$FlightView->Busy;
    $file=~ /(.+)\/(.+?)$/;     #Filename nach letztem "/" in $2
    my $title=$2;       	### titel fuer plot

    if (defined $title) {} else {$title="No Title";}

    print BPIPE "set title \"$title\"\n";   ### wird gesetzt

    if ($mode eq '3d') 	{
        print BPIPE "set view ".(90-$config{'gnuplot_3d_rotx'}).",".(360-$config{'gnuplot_3d_rotz'})."\n";
        print BPIPE "set ticslevel 0\n";
    }

    if ($config{'gnuplot_grid_state'} eq 'set grid')	{
        print BPIPE "set grid\n";
    }  ### grid on

    print BPIPE "set title \"$file\"\n";
    print BPIPE $com1." $bereich ".$com2."\n";

    print  $com1." $bereich ".$com2."\n" if ($config{'DEBUG'});

    close (BPIPE) if ($^O ne "MSWin32" && $config{'gnuplot_major_version'} == 3);

    if ($^O eq "MSWin32") {
        close (BPIPE);
        open (BATCH, ">${pfadftmp}plot.bat");
        print BATCH "\@ECHO off\n";
        print BATCH "$config{'gnuplot_win_exec'} ${pfadftmp}plot.gpl $w32_persist\n";
        close (BATCH);
        system("start $start_parameter ${pfadftmp}plot.bat"); # windows trick ;-)
    } elsif ($config{'gnuplot_major_version'} == 4) {

        # here new start of gnuplot for unices
        # if gnuplot==4
        #print BPIPE "bind x \"quit\"\n";
        print BPIPE "pause -1 \n" if (!$config{'gnuplot_4_terminal'});

        #print BPIPE "exit \n";
        close (BPIPE);
        my $pidf = fork();
        if ($pidf == 0) {
            print "starting gnuplot 4 forked!\n" if ($config{'DEBUG'});
            system( "gnuplot ${pfadftmp}plot.gpl $w32_persist $linuxquiet") if (!$config{'gnuplot_4_terminal'});
            system( "$config{'gnuplot_terminal_app'} \'gnuplot ${pfadftmp}plot.gpl $w32_persist\'") if ($config{'gnuplot_4_terminal'});

            #use POSIX ":sys_wait_h";
            #waitpid($pidgp,NULL);
            #close(GP);
            wait();
            CORE::exit();
        }

    }

    $savname='no'; ##nach ausgabe savname auf 'no' damit next time ein neuer name abgefragt wird
    #$bereich=''; ### bereich loeschen!
    $busy=$FlightView->Unbusy;
}

#  gnuplot
###############################################################################

# give out an error message
sub Errorbox {
    my $fm=$_[0];
    my $Errorbox=$FlightView->messageBox(-message=>$fm, -type=>OK, -icon=>'error');
    return ;
}


# Errorbox
###############################################################################

sub Photodirselect {

    if (!$config{'photos'}) {return;}

    my $initdir = $config{'working_directory'};

    if ($config{'photo_path'} ne "none") {
        $initdir = $config{'photo_path'};
    }

    my $types = [
        ['JPEG photos',   ['.jpg', '.JPG', '.JPEG', '.jpeg']],
        ['Audio',   ['.amr', '.mp3', '.AMR', '.MP3','.ogg','.OGG']],
        ['Video',   ['.3gp', '.3GP', '.avi', '.AVI']],
        ['All Files',   '*' , ],
      ];

    my $dir = $FlightView->getOpenFile(-initialdir=>$initdir,-title=>"Open photos",-filetypes=>$types);

    if (defined $dir) {
        $dir = GPLIGCfunctions::setpwd($dir);
        if (-d $dir) {
            $config{'photo_path'}= $dir;

            checkPhotos(1); # 1 = flag, that we call from Photodirselect and photo_path should be used first!
            processPhotos();

        }
    }

    return;

}

# Fileselector for opening (called from main loop)
sub Fileselekt {

    my $types = [
        ['IGC Files',   ['.igc', '.IGC']],
        ['All Files',   '*'  ]
      ];

    my $pwd_winsafe = $config{'working_directory'};

    if ($^O eq "MSWin32") {
        $pwd_winsafe =~ s#/#\\#g;
    }

    my $file = $FlightView->getOpenFile(-initialdir=>$pwd_winsafe,-title=>"Open IGC file",-filetypes=>$types);

    if (defined $file) {
        schliessen();
        $config{'working_directory'}=GPLIGCfunctions::setpwd($file);
        oeffnen($file);

        updateFVW('i');

    }
    return;
}

# Fileselekt
###############################################################################

###Fileselector f.saving files
sub save {
    my $vorschlag = shift;
    if (!defined $vorschlag) {$vorschlag="";}
    my $sfile=$FlightView->getSaveFile(-initialfile=>$vorschlag,-initialdir=>$config{'working_directory'}, -title=>"Save output as");

    if (defined $sfile) {
        $config{'working_directory'}=GPLIGCfunctions::setpwd($sfile);

# thats tested by getSaveFile already and asked for overwriting!
#if (open (TEST,"<$sfile")) {close (TEST);  Errorbox("File exists"); return ('no');}
        if (open (TEST,">$sfile")) {
            close (TEST);
            unlink($sfile);
        } else {
            close (TEST);  Errorbox("Cannot write"); return('no');
        }
    } else {
        $sfile='no';
    }

    return ($sfile);
}

sub directory {
    my $sfile=$FlightView->chooseDirectory(-initialdir=>$config{'working_directory'}, -title=>"Choose Directory");

    if (defined $sfile && $file ne "") {
        $config{'working_directory'}=$sfile;

        return ($sfile);
    } else {

        return ("no");
    }

}

###############################################################################

###schliessen
# used if new file is opened
sub schliessen {

    # clear temp files
    foreach (@clearlist) {
        unlink($pfadftmp.$_);
    }

    #$calcstat='no';
    if ($filestat eq 'closed'){return;}
    $filestat='closed';
    $file='No file opened';

    #close additional windows
    if (Exists($FF)) {$FF->destroy;}
    if (Exists($FI)) {$FI->destroy;}
    if (Exists($MF)) {$MF->destroy;}
    if (Exists($WPPlotWindow)) {$WPPlotWindow->destroy;}
    if (Exists($thermal_statistik)) {$thermal_statistik->destroy;}
    if (Exists($glide_statistik)) {$glide_statistik->destroy;}

    #check for input-windows?!

    # clear drawing
    if (@trackplot) {$canvas -> delete (@trackplot); undef @trackplot;}
    if (@mapplot) {$canvas -> delete (@mapplot); undef @mapplot;}
    if (@wpcyl_lines) {$canvas -> delete (@wpcyl_lines); undef @wpcyl_lines;}
    if (@acc_lines) {$canvas -> delete (@acc_lines); undef @acc_lines;}
    if (@task_lines) {$canvas -> delete (@task_lines); undef @task_lines;}
    if (@baroplot) {$barocanvas -> delete (@baroplot); undef @baroplot;}
    if (@baroplot_task) {$barocanvas -> delete (@baroplot_task); undef @baroplot_task;}
    if (@marks_lines) {$canvas -> delete (@marks_lines); undef @marks_lines;}
}

###############################################################################

### subroutine ende. alle erzeugten *.dat werden geloescht
sub gpligcexit {

    # remove temporary files
    foreach (@clearlist) {
        unlink($pfadftmp.$_);
    }

    rmdir $pfadftmp;

    # remove the shared memory segment
    #	if ($^O ne "MSWin32") {
    #		if (defined $shm_id) {
    #			shmctl($shm_id, IPC_RMID, 0) || print "Error removing shared mem\n";
    #			}
    #	}

    exit;

}

###############################################################################

sub FVWsetInfo {
    @introtext=();
    push(@introtext, $canvas->createText(5,5, -anchor=>"nw", -text=>$text, -fill=>"black"));
    $canvas->update();
}

### subroutine info
sub info {

    # jpg if Tk::JPEG availabel, otherwise gif
    my $pictype;
    my $picname = "logos";
    if ($have_jpeg) {
        $pictype = "jpg";
        print "Tk::JPEG available. Trying to use jpeg-logo!\n" if ($config{'DEBUG'});
    } else {
        $pictype = "gif";
        print "Tk::JPEG not available. Trying to use gif-logo!\n" if ($config{'DEBUG'});
    }

    if (Exists($infoFenster)) {$infoFenster->destroy;}

    $infoFenster=$FlightView->Toplevel();
    $infoFenster->configure(-title=>"Information");

    setexit($infoFenster);
    my $pic=$infoFenster->Photo();

    # Look in install-directory AND in ./ for appropriate files...
    if(-f "$datadir/$picname.$pictype"){
        $pic->configure(-file=>"$datadir/$picname.$pictype");
    } elsif (-f "./$picname.$pictype"){
        $pic->configure(-file=>"./$picname.$pictype");
    } elsif(-f "${scriptpath}$picname.$pictype") {
        $pic->configure(-file=>"${scriptpath}$picname.$pictype");
    } else {

        print "Could not find \'$picname.$pictype\' in $datadir/ and ./ and $scriptpath Fix this, please. Exiting now...\n";
        gpligcexit(1);
    }

    my $picbut=$infoFenster->Label(-image=>$pic);
    $picbut->pack();

    my $mess=$infoFenster->Message(-width=>"80c",-text=>$text);
    $mess->pack();
    my $exb=$infoFenster->Button(-text=>"Exit",-command=>sub{$infoFenster->destroy();});
    $exb->pack();
}

sub help {

    # jpg if Tk::JPEG availabel, otherwise gif
    if (Exists($helpFenster)) {$helpFenster->destroy;}

    $helpFenster=$FlightView->Toplevel();
    $helpFenster->configure(-title=>"Help / Keys");

    setexit($helpFenster);

    my $labeltext= <<ENDE;
Move cursor:
    <<(F1) <(F2) (F3)> (F4)>>

Measuring/select tool:
    (F5)=Set first point, (F6)=Set second point, (F7)=Measure
    (F11)=zoom to selection, (F12)=reset selection

Statistics:
    (F8)=Thermal statistics, (F9)=Glide statistics

Set task markers:
    (s)start, (f)inish

Tasks:
    (t)=on/off, add (w)aypoint, insert waypoint (b)efore / (a)fter actual WP, re(p)lace WP

Waypoints:
    (c)ylinder on/off, set (r)ange

Output:
    (o)utput to postscript, (i)=baro, (F10)=save point to .lif file

Altitude:
    (e)levation calibration, Q(n)H calibration

Drawing:
    (g)rid on/off, (y) resize/redraw, (z)oom on/off, zoom to (T)ask
    (M)aps on/off, (#)=re-download maps, (+)/(-) change map zoom level
    (F11)=zoom to selected

ENDE

    if ($config{'photos'}) {
        $labeltext .= "Photos / Multimedia:\n    (v)iew next photo/mm, e(x)act photo/mm time calibration\n    (h)ide photos/mm, (m)edia list, geotag pict(u)res\n\n";
    }

    $labeltext .= "(ESC) Exit";

    my $mess=$helpFenster->Message(-text=>$labeltext); #-width=>"80c",-text=>$labeltext);
    $mess->pack();
    my $exb=$helpFenster->Button(-text=>"Exit",-command=>sub{$helpFenster->destroy();});
    $exb->pack();
}

###############################################################################

sub OpenGLexplorer {
    if ($filestat eq 'closed') {$FlightView->bell; return;}

    # we may have more that one...
    #	if ($^O ne "MSWin32") {
    #		if (defined $oGLePID) {return}
    #	}

    if (system("ogie --check")) {Errorbox("Can't execute \"ogie\". Check installation");
        return;}

    # We start the explorer with --parent-pid $$, it will send a SIGUSR1 on exit
    # On receiving SIGUSR1 we will waitpid for it (we dont need a zombie...)
    # but not on windows

    my $DEBUGOGIE = "";
    $DEBUGOGIE = "--debug" if ($config{'DEBUG'});

    my $lifts="";
    $liftfile = substr($file, 0, -3)."lif";
    if (-r $liftfile) {$lifts= "--lifts \"$liftfile\"";}

    # so far lifts or photos
    else {
        if ($config{'photos'} && !$hide_mm && $#PHOTO_FILES > 0) {

            $pfile = substr($file, 0, -3)."photocoord";
            open (PF, ">$pfile");

            for (my $z=0; $z<=$#PHOTO_FILES;$z++) {
                my ($picname, $p, $suf) = File::Basename::fileparse($PHOTO_FILES[$z]);
                print PF "$DECLAT[$PHOTO_FILES_INDICES[$z]] $DECLON[$PHOTO_FILES_INDICES[$z]] $BARO[$PHOTO_FILES_INDICES[$z]] 0 0 0 0 $picname \n";
            }
            close PF;

            # ausgabe pseudo-liftsfile
            $lifts = "--lifts-info-mode 7 --lifts \"$pfile\" ";
        }
    }

    if ($^O ne "MSWin32") {

        # this produced the warning

        $oGLePID = open (OGLEX, "| ogie $DEBUGOGIE -i \"$file\" --parent-pid $$ --marker-pos $nr --marker --offset $offset --quiet $lifts ");

#		$oGLePID = open (OGLEX, "| ogie $DEBUGOGIE -i \"$file\"  --marker-pos $nr --marker --offset $offset --quiet");
        print "OGIE PID is : $oGLePID \n" if ($config{'DEBUG'});

        #close (OGLEX);

# -> Don't forget to waitpid somewhere.... (what formerly was don on receiving SIGUSR1, see below)

        ### This was the old SysV IPC communication (shared memory)
        ### not available on many platforms (win32)
        ### SIGNAL handling "too" safe in perl for that reason
        ### no signal processing in MainLoop, when idle.

        $SIG{'USR1'} = sub {waitpid $oGLePID,0; close OGLEX; undef $oGLePID;

        #			shmctl($shm_id, IPC_RMID, 0) || print "Error removing shared mem\n";
        #			undef $shm_id;
          };
        #
        ## what to do on receiving sigusr2, sigusr2 is send by openGLIGCexplorer, when marker is moved...
#$SIG{'USR2'} = sub {$sigusr2flag=1; print "got sigusr2.. (GPLIGC)\n"; FVWausg();$FlightView->update;};
#
        ## After we startet the explorer we can initialize the SysV IPC shared Memory.....
        ## 76 is a magic number
        #$shm_key = ftok($file, 76);
        #
        ##print "SHM_key vom GPLIGC = $shm_key \n";
#$shm_id = shmget($shm_key , 4, IPC_CREAT | S_IRWXU);
#
#if (!defined $shm_id) {
#	print "Error initializing the SysV IPC Shared Memory.... No communication with openGLIGCexplorer :-(\n";

        #}

    } else { # starting openGLIGCexplorer for windows

        system("start $start_parameter ogie $DEBUGOGIE -i \"$file\" --marker-pos $nr --marker --offset $offset --quiet $lifts ");

#print "start /B ogie -i $file --marker-pos $nr --marker --offset $offset --quiet";
    }

    ###
    # -> Here goes the new sockets-code
    # start a listening tcp server  (non-blocking mode)
    # with fileevent-> callback for recieving and updating

    # we need a flag to decide if explorer is running
    # and we need to send the explorer positions also.
    # ............

}

########################################

### FlightInfo
sub FlightInfo {
    if (Exists($FI)) {$FI->destroy;}

    if ($filestat eq 'closed') {$FlightView->bell; return;}
    $FI=$FlightView->Toplevel();
    $FI->configure(-title=>"Flight information");

    setexit($FI);

    $FDR = "unknown" if !defined $FDR;
    my $finfotext="Flight data recorder:\n$FDR\nHeader:\n";

    foreach (@HEADER) {
        $_=~/(.*?)(\s*)$/;
        $finfotext.=$1."\n";
    }

    foreach (@TASK_info) {
        $finfotext.=$_."\n";
    }

    my $label=$FI->Message(-width=>"80c",-textvariable=>\$finfotext);
    $label->pack(-fill=>"x");

    my $exb=$FI->Button(-text=>"Exit",-command=>sub{$FI->destroy();});
    $exb->pack();
}

###############################################################################

### Flight Statistics
sub FlightStatistics {
    if (Exists($FF)) {$FF->destroy;}

    if ($filestat eq 'closed') {$FlightView->bell; return;}
    $FF=$FlightView->Toplevel();
    $FF->configure(-title=>"Flight Statistics");

    $fstattext='';

    setexit($FF);

    FSupdate();

    my $label=$FF->Message(-width=>"80c",-textvariable=>\$fstattext);
    $label->pack();

    my $exb=$FF->Button(-text=>"Exit",-command=>sub{$FF->destroy();});
    $exb->pack();

    print "Flight statistics:\n".$fstattext  if ($config{'DEBUG'});
}

################################################################################

sub FSupdate {
    if (!Exists($FF)) {return;}

    $fstattext='';

    my $starttimeindex=GPLIGCfunctions::takeoff_detect(\@nonISPEED, 's');
    my $landtimeindex=GPLIGCfunctions::takeoff_detect(\@nonISPEED, 'l');

    my $stime=GPLIGCfunctions::time2human($TIME[$starttimeindex], 't');
    my $ltime=GPLIGCfunctions::time2human($TIME[$landtimeindex], 't');

    $fstattext.="Takeoff: $stime\nLanding: $ltime\n";

    my $decflighttime=$DECTIME[$landtimeindex]-$DECTIME[$starttimeindex];

    my $flighttime=GPLIGCfunctions::dec2time($decflighttime);
    $fstattext.="Time of flight: $flighttime\n";

    my ($maxalt,$maxpalt,$t100,$t125,$t125acc,$t140,$t180,$l100,$l125,$l125acc,$l140,$l180,$free) = GPLIGCfunctions::OxygenStatistics(\@BARO,\@DECTIME,$gpi{'qnh'});

    $fstattext.="Maximum baro altitude: ".sprintf("%.0f",$maxalt*$config{'altitude_unit_factor'})." ".$config{'altitude_unit_name'}."\n";

    $fstattext.="Maximum press altitude: ".sprintf("%.0f",$maxpalt*$config{'altitude_unit_factor'})." ".$config{'altitude_unit_name'}." (assuming QNH $gpi{'qnh'})\n";

    $fstattext.="Maximum GPS altitude: ".sprintf("%.0f",$GPSALT_max*$config{'altitude_unit_factor'})." ".$config{'altitude_unit_name'}."\n-----\n";

    $fstattext.="Oxygen debriefing, FAR 91.211 (constant open flow systems)\n";
    $fstattext.="FL100 < alt < FL125: ".sprintf("%.0f",$t100*60)." min  ".sprintf("%.1f",$l100)." l (".sprintf("%.1f",$l100/3)." l) \n";
    $fstattext.="FL125 < alt < FL140: ".sprintf("%.0f",$t125*60)." min  ".sprintf("%.1f",$l125)." l (".sprintf("%.1f",$l125/3)." l),  FAA: ".sprintf("%.0f min %.1f l (%.1f l)\n",$t125acc*60, $l125acc,$l125acc/3);

    $fstattext.="FL140 < alt < FL180: ".sprintf("%.0f",$t140*60)." min  ".sprintf("%.1f",$l140)." l (".sprintf("%.1f",$l140/3)." l) \n";

    $fstattext.="FL180 < alt < ORBIT: ".sprintf("%.0f",$t180*60)." min  ".sprintf("%.1f",$l180)." l\n";

    my $rec= $l100+$l125+$l140+$l180;
    my $rec_oxy = $l100/3+$l125/3+$l140/3+$l180;
    my $faa = $l125acc+$l140+$l180;
    my $faa_oxy= $l125acc/3+$l140/3+$l180;

    $fstattext .= sprintf("Oxygen totals:\nRecommended: %.1f l (%.1f l)\n",$rec,$rec_oxy);
    $fstattext .= sprintf ("FAA: %.1f l (%.1f l)\n",$faa,$faa_oxy);

    $fstattext.="-----\n"; #5

    # sum all legs (plain task)
    my $entf=0;
    my @entf=();
    $entf[0]=0;

    for (my $zaehl=1 ; $zaehl<=$#WPLAT-1; $zaehl++) {
        $entf[$zaehl]=GPLIGCfunctions::dist($WPLAT[$zaehl],$WPLON[$zaehl],$WPLAT[$zaehl+1],$WPLON[$zaehl+1]);
    }

    foreach (@entf) {
        $entf+=$_;
    }

    #print "$task_start_time \n $task_finish_time \n";

    my $taskstt=GPLIGCfunctions::time2human($TIME[$task_start_time_index],'t');
    my $taskft=GPLIGCfunctions::time2human($TIME[$task_finish_time_index],'t');

    $fstattext.="Begin of task/unpowered flight (s-marker): $taskstt \nEnd of task/unpowered flight (f-marker): $taskft\n";
    $task_time=GPLIGCfunctions::dec2time($DECTIME[$task_finish_time_index]-$DECTIME[$task_start_time_index]);

    $fstattext.="Task time (unpowered flight time): $task_time\n";

    $dec_task_time=$DECTIME[$task_finish_time_index]-$DECTIME[$task_start_time_index];

    if ($dec_task_time != 0) { $task_speed= $entf / $dec_task_time ;} else { $task_speed = "42";} #this is an egg :)

    $fstattext.= "-----\n"; #5

    # calculate partial distances
    my @anteil=();
    $anteil[0]=0;
    my @anteil_ohne_null=();

    #bestimmung der entfernungen und anteile zwischen allen wp
    for (my $zaehl=1 ; $zaehl<=$#WPLAT-1; $zaehl++) {
        $anteil[$zaehl] = ($entf[$zaehl]/$entf)*100  if $entf != 0;
        if ($entf == 0) {$anteil[$zaehl] = 0;}
        $anteil[$zaehl] = sprintf ("%5.2f",$anteil[$zaehl]);
        my $p_entf=sprintf ("%6.2f",$entf[$zaehl]*$config{'distance_unit_factor'});
        $fstattext.="$anteil[$zaehl] % $p_entf $config{'distance_unit_name'} $WPNAME[$zaehl] -> $WPNAME[$zaehl+1]\n";
    }

    my $print_entf=sprintf("%.4f", $entf*$config{'distance_unit_factor'});

    $fstattext.="Taskdistance: $print_entf $config{'distance_unit_name'}\n";

    my $task_speed_rounded = sprintf ("%.2f",$task_speed*$config{'speed_unit_factor'});
    $fstattext.="Task speed: $task_speed_rounded $config{'speed_unit_name'}\n";

    #BESTIMMUNG OB DREIECK UND FAI!!!

    #How many legs has the task?
    my $z = 0;

    # z ist die anzahl der schenkel mit anteil > 0
    foreach (@anteil) {
        $z++ if $_ != 0;
    }

    # calculate task for triangular task
    $entf = 0;
    @entf=();
    $entf[0]=0;

    if ($#WPLAT >= 4 ) {
        for (my $z=2; $z<4; $z++) {
            $entf[$z-1]=GPLIGCfunctions::dist($WPLAT[$z],$WPLON[$z],$WPLAT[$z+1],$WPLON[$z+1]);
            $entf+=$entf[$z-1];
        }

        $entf[3] = GPLIGCfunctions::dist($WPLAT[4],$WPLON[4],$WPLAT[2],$WPLON[2]);
        $entf+=$entf[3];

        # triangular task
        if (  (($z == 4) && ($WPNAME[1] eq $WPNAME[5])) || (($z == 4) &&  (GPLIGCfunctions::dist($WPLAT[1],$WPLON[1],$WPLAT[5],$WPLON[5]) <= 0.2 * $entf) ) ) {

            $fstattext.="\n-----\nTriangular task (start and finish on leg)\n";

            my $print_close = (GPLIGCfunctions::dist($WPLAT[1],$WPLON[1],$WPLAT[5],$WPLON[5])/$entf)*100;
            $print_close = sprintf("%.1f",$print_close);

            my $print_close_dist = GPLIGCfunctions::dist($WPLAT[1],$WPLON[1],$WPLAT[5],$WPLON[5]) * $config{'distance_unit_factor'};
            my $print_close_distr = sprintf("%.4f",$print_close_dist);
            my $task_m_close = sprintf("%.4f", $entf - $print_close_dist);

            $fstattext.="Closing distance (between start and finish): ".$print_close_distr." $config{'distance_unit_name'}  ".$print_close."% of triangular task\n";

            @anteil=();
            $anteil[0]=0;

            for (my $zaehl=2 ; $zaehl<4; $zaehl++) {
                $anteil[$zaehl-1] = ($entf[$zaehl-1]/$entf)*100 if $entf != 0;
                $anteil[$zaehl-1] = sprintf ("%5.2f",$anteil[$zaehl-1]);
                my $p_entf =sprintf ("%6.2f",$entf[$zaehl-1]*$config{'distance_unit_factor'});

                $fstattext.="$anteil[$zaehl-1] % $p_entf $config{'distance_unit_name'} $WPNAME[$zaehl] -> $WPNAME[$zaehl+1]\n";

            }

            $anteil[3] = ($entf[3]/$entf)*100 if $entf != 0;
            $anteil[3] = sprintf ("%5.2f",$anteil[3]);
            my $p_entf =sprintf ("%6.2f",$entf[3]*$config{'distance_unit_factor'});

            $fstattext.="$anteil[3] % $p_entf $config{'distance_unit_name'} $WPNAME[4] -> $WPNAME[2]\n";

            my $p2_entf=sprintf("%.4f", $entf * $config{'distance_unit_factor'});
            $fstattext.="Triangular task distance: $p2_entf $config{'distance_unit_name'}\n";
            if ($dec_task_time != 0) { $task_speed= $entf / $dec_task_time ;} else { $task_speed = "42";} #this is an egg :)
            my $task_speed_rounded = sprintf ("%.2f",$task_speed * $config{'speed_unit_factor'});
            $fstattext.="Triangular task speed: $task_speed_rounded $config{'speed_unit_name'}\n";
            $fstattext.="Triangular task distance - closing: $task_m_close\n";
        }
    }

    #check for FAI Task
    #we need an array of the percentages, but without the ones which are zero
    for (my $run=0; $run <=$#anteil; $run++) {
        if ($anteil[$run] != 0) {push(@anteil_ohne_null,$anteil[$run]);}
    }

    #check if task is triangular...
    my $TASKSHAPE = 'unknown';

    # z==3: these can only be a predeclared task, (or defined by hand? :(  )
    if (($z == 3) && (($WPNAME[1] eq $WPNAME[4]) || (GPLIGCfunctions::dist($WPLAT[1],$WPLON[1],$WPLAT[4],$WPLON[4]) <= 1.0)  )  ){$TASKSHAPE='triangular';}

    # z==4 this should be an optimized task, we will assume the "softest" closing criteria (holc)
    if (($z == 4) && (($WPNAME[1] eq $WPNAME[5]) || (GPLIGCfunctions::dist($WPLAT[1],$WPLON[1],$WPLAT[5],$WPLON[5]) <= 0.2 * $entf)  )  ){$TASKSHAPE='triangular';}

    if ($TASKSHAPE eq 'triangular')  {		#Task has 3 legs and closed    FAI?
        my ($max, $min) = GPLIGCfunctions::MaxKoor(\@anteil_ohne_null);   # can be used to get maximum and minimum... :)
        #print "$min $max\n";
        $fstattext.="Triangular task ";

        if ($entf < 500) {   #kleiner 500
            #print "kleiner 500\n";
            if ($min >= 28) {$fstattext.= "with FAI record shape";}
        }

        if ($entf >= 500) {

            #print "guter pilot, ueber 500! :) \n";
            if ($min > 25 && $max <=45) {$fstattext.= "with FAI record shape";}
        }

        $fstattext.="\n";
    }

    $fstattext.="-----\nFlown task:\n";
    my ($wpinxref, $wpnameref) = getWPreachedIndices();

    for (my $idx=0; $idx < $#{$wpinxref}; $idx++) {

        my $legdist = GPLIGCfunctions::dist($DECLAT[$wpinxref->[$idx]], $DECLON[$wpinxref->[$idx]],$DECLAT[$wpinxref->[$idx+1]], $DECLON[$wpinxref->[$idx+1]]);
        my $dtime = $DECTIME[$wpinxref->[$idx+1]]-$DECTIME[$wpinxref->[$idx]];

        my $legspeed;
        if ($dtime !=0) { $legspeed=$legdist / $dtime;} else {$legspeed = 9999;}
        my $legaltdiff = $BARO[$wpinxref->[$idx+1]]-$BARO[$wpinxref->[$idx]];
        my $legtime = GPLIGCfunctions::time2human($TIME[$wpinxref->[$idx]],'t')."-".GPLIGCfunctions::time2human($TIME[$wpinxref->[$idx+1]],'t');
        my $legglide;
        if ($legaltdiff !=0 ){ $legglide =  ($legdist * 1000) / -$legaltdiff;} else {$legglide= 9999;}
        print "$legglide\n" if ($config{'DEBUG'});

        $legdist *= $config{'distance_unit_factor'};
        $legaltdiff *= $config{'altitude_unit_factor'};
        $fstattext.= sprintf("%1.0f:  %4.2f $config{'distance_unit_name'} %3.0f $config{'speed_unit_name'} $legaltdiff $config{'altitude_unit_name'} %.0f $legtime $wpnameref->[$idx] -> $wpnameref->[$idx+1]\n",$idx+1, $legdist,$legspeed, $legglide);

    }

}

##############################################################################

sub zylinder {
    use Math::Trig;
    my ($lat, $lon, $r, $h) = @_; ###r=durchmesser
    my $gpx;

    Ausschnitt($lat, $lon);

    #if ($r eq "point") {$gpx=0.0008993216;} ### 100m for point-like marks
    #if ($r eq "200m") {$gpx=0.0017986432;}   ### /0.3km on great-circle on FAI-Spheroid
    #if ($r eq "300m") {$gpx=0.002697964;}   ### /0.3km on great-circle on FAI-Spheroid
    #if ($r eq "400m") {$gpx=0.0035972864;}   ### /0.3km on great-circle on FAI-Spheroid
    #if ($r eq "500m") {$gpx=0.004496608;}   ### /0.5km on great-circle on FAI-Spheroid
    #if ($r eq "1km") {$gpx=0.0089932161;}
    $gpx=0.0089932161 * $r;
    #if ($r eq "FAI") {$gpx=0.026979648;}    ### 3 km for FAI sector
    #if ($r eq "off") {$gpx=0;}

    my @dlat=();
    my @dlon=();

    for (my $a=0;$a<=6.283185307;$a=$a+0.0314) {
        push(@dlat, cos($a)*$gpx);
        push(@dlon, sin($a)*$gpx*(1/cos(deg2rad($lat))));
    }

    open (ZYLOUT,">${pfadftmp}zyl.dat");

    for ($i=0; $i<=$#dlat; $i++) 	{
        for ($j=0; $j<=$h; $j=$j+100)	{  ###make it a zylinder!
            print ZYLOUT $lon+$dlon[$i]," ",$lat+$dlat[$i]," $j \n";
        }
    }

    close ZYLOUT;
}

# end zylinder

##############################################################################################################################################
##############################################################################################################################################
################################# FLIGHTVIEW #################################################################################################
sub FlightView {

    if ($config{'fvw_baro_fraction'} < 1.1 || $config{'fvw_baro_fraction'} > 10) {
        print ("fvw_baro_fraction should be larger than 1.1 and smaller than 10\n");
        gpligcexit();
    }

    $FlightView = MainWindow->new();
    $FlightView->configure(-title=>"GPLIGC $version");

    # this catches wm-exit-button, wm-exit...
    $FlightView->protocol('WM_DELETE_WINDOW', =>\&gpligcexit);

    $FlightView->wm("iconname", "GPLIGC");

    #$Fenster->resizable(0, 0);

    # CREATE AND pack menu bar
    my $menuleiste=$FlightView->Frame(-relief=>'solid', -borderwidth=>1);
    my $menu_file=$menuleiste->Menubutton(-text=>"File");#, -underline=>0);
    $menu_file->command(-label=>"Open", -command=>sub{Fileselekt();});

    $menu_file->command(-label=>"Reload", -command=>sub{
            if ($filestat ne 'closed') { oeffnen($file);FVWausg();FSupdate();}
          });

    $menu_file->command(-label=>"Download track (gpsbabel)", -command=>sub{download_gpsbabel();});

    if ($^O ne "MSWin32") {
        $menu_file->command(-label=>"Download garmin (gpspoint)", -command=>sub{download_garmin();});
        $menu_file->command(-label=>"Download media", -command=>sub{download_media();});
    }

    $menu_file->command(-label=>"Export kml", -command=>sub{kml_export();});
    $menu_file->command(-label=>"Export gpx", -command=>sub{gpx_export();});

    # this is not needed.
    #$menu_file->command(-label=>"Close",-command=>sub{schliessen();});

#    if ($config{'photos'}) {
        $menu_file->command(-label=>"Open photo/multimedia directory", -command=>sub{
                if ($filestat ne 'closed') {
                    Photodirselect();}
              });
#    }

    $menu_file->separator();
    $menu_file->command(-label=>"Exit",-command=>\&gpligcexit);
    $menuleiste->pack(-side=>'top', -fill=>'x',-padx=>3);
    $menu_file->pack(-side=>'left');

    my $menu_about=$menuleiste->Menubutton(-text=>"About");
    $menu_about->command(-label=>"Help",-command=>\&help);
    $menu_about->command(-label=>"Info",-command=>\&info);
    $menu_about->command(-label=>"GPLIGC home",-command=>sub{browseURL("http://gpligc.sf.net");});
    $menu_about->command(-label=>"Subscribe mailinglist",-command=>sub{browseURL("https://lists.sourceforge.net/lists/listinfo/gpligc-announce");});
    $menu_about->command(-label=>"Get support / report bugs",-command=>sub{browseURL("https://sourceforge.net/projects/gpligc/support");});

#$menu_about->command(-label=>"donate money",-command=>sub{browseURL("https://sourceforge.net/donate/index.php?group_id=101635");});
    $menu_about->pack(-side=>"right");

    my $menu_options=$menuleiste->Menubutton(-text=>"Options");#,-underline=>0);

    $menu_options->separator();

    $mapmenu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"Map settings", -menu=> $mapmenu);

    $mapmenu->checkbutton(-label =>"Use maps", -variable=>\$config{'maps'},-command=>sub{if (Exists($FlightView)) {updateFVW('i');}   });
    $mapmenu->radiobutton(-label => "Open street map",-variable => \$config{'map_type'},-value => "osm",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });
    $mapmenu->radiobutton(-label => "Open cycle map",-variable => \$config{'map_type'},-value => "osmC",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });
    $mapmenu->radiobutton(-label => "Open flight map (merged)",-variable => \$config{'map_type'},-value => "ofm-m",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });
    $mapmenu->radiobutton(-label => "Open flight map (aero)",-variable => \$config{'map_type'},-value => "ofm-a",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });
    $mapmenu->radiobutton(-label => "Open flight map (base)",-variable => \$config{'map_type'},-value => "ofm-b",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });
    #$mapmenu->radiobutton(-label => "Open Sea map",-variable => \$config{'map_type'},-value => "oseam",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });
    $mapmenu->radiobutton(-label => "Gmaps",-variable => \$config{'map_type'},-value => "gmm",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });
    $mapmenu->radiobutton(-label => "Gmaps terrain",-variable => \$config{'map_type'},-value => "gmt",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });
    $mapmenu->radiobutton(-label => "Gmaps satellite",-variable => \$config{'map_type'},-value => "gms",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });
    $mapmenu->radiobutton(-label => "Gmaps hybrid",-variable => \$config{'map_type'},-value => "gmh",-command=>sub{if (Exists($FlightView)) {updateFVW();}   });

    #$menu_options->seperator();

    $gpmenu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"Gnuplot settings", -menu=> $gpmenu);

      if ($^O ne "MSWin32") {
        $gpmenu->radiobutton(-label=>"Gnuplot 4.x",-variable=>\$config{'gnuplot_major_version'},-value=>"4");
        $gpmenu->radiobutton(-label=>"Gnuplot 3.x",-variable=>\$config{'gnuplot_major_version'},-value=>"3");
        $gpmenu->radiobutton(-label=>"Open Gnuplot-shell",-variable=>\$config{'gnuplot_4_terminal'},-value=>"1");
        $gpmenu->radiobutton(-label=>"No Gnuplot-shell",-variable=>\$config{'gnuplot_4_terminal'},-value=>"0");
        $gpmenu->separator();
    }

    $gpmenu->radiobutton(-label=>"Grid on",-variable=>\$config{'gnuplot_grid_state'},-value=>"set grid");
    $gpmenu->radiobutton(-label=>"Grid off",-variable=>\$config{'gnuplot_grid_state'},-value=>'');

    $gpmenu->separator;

    $drawmenu = $gpmenu->Menu();
    $gpmenu->cascade(-label=>"Draw options", -menu=> $drawmenu);

    $drawmenu->radiobutton(-label => "Lines",-variable => \$config{'gnuplot_draw_style'},-value => "with lines");
    $drawmenu->radiobutton(-label => "Dots",-variable => \$config{'gnuplot_draw_style'},-value => "with dots");
    $drawmenu->radiobutton(-label => "Linespoints",-variable => \$config{'gnuplot_draw_style'},-value => "with linespoints");

    $termmenu = $gpmenu->Menu();
    $gpmenu->cascade(-label=>"Gnuplot terminal", -menu=> $termmenu);

    if ($^O ne "MSWin32") {
     $termmenu->radiobutton(-label => "x11",-variable => \$config{'gnuplot_terminal'},-value => "x11");
     $termmenu->radiobutton(-label => "qt",-variable => \$config{'gnuplot_terminal'},-value => "qt");
    } else {
     $termmenu->radiobutton(-label => "screen",-variable => \$config{'gnuplot_terminal'},-value => "x11"); # is being ignored. no 'set term' in win32
    }
    $termmenu->radiobutton(-label => "pngcairo",-variable => \$config{'gnuplot_terminal'},-value => "pngcairo");
    $termmenu->radiobutton(-label => "epscairo",-variable => \$config{'gnuplot_terminal'},-value => "epscairo");
    $termmenu->radiobutton(-label => "pdfcairo",-variable => \$config{'gnuplot_terminal'},-value => "pdfcairo");

    $termmenu->radiobutton(-label => "png",-variable => \$config{'gnuplot_terminal'},-value => "png");
    $termmenu->radiobutton(-label => "ps color",-variable => \$config{'gnuplot_terminal'},-value => "postscript color solid");
    $termmenu->radiobutton(-label => "eps color",-variable => \$config{'gnuplot_terminal'},-value => "postscript eps color solid");
    $termmenu->radiobutton(-label => "fig monochrome",-variable => \$config{'gnuplot_terminal'},-value => "fig monochrome");
    $termmenu->radiobutton(-label => "fig color",-variable => \$config{'gnuplot_terminal'},-value => "fig color");

    $optmenu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"Optimizer method", -menu=> $optmenu);
    $optmenu->radiobutton(-label => "Metropolis Montecarlo",-variable => \$config{'optimizer_method'},-value => "mmc");
    $optmenu->radiobutton(-label => "Simulated Annealing",-variable => \$config{'optimizer_method'},-value => "sa");

    $zylmenu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"WP Cylinder/Sector", -menu=> $zylmenu);

    $zylmenu->radiobutton(-label => "FAI Sectors", -variable=>\$config{'zylinder_wp_type'}, -value=>"sec",-command=>\&updateCyl);
    $zylmenu->radiobutton(-label => "Cylinders",-variable=>\$config{'zylinder_wp_type'},-value=>"cyl",-command=>\&updateCyl);
    $zylmenu->radiobutton(-label => "FAI Sec and Cylinders", -variable=>\$config{'zylinder_wp_type'}, -value=>"both", -command=>\&updateCyl);
    $zylmenu->radiobutton(-label => "None", -variable=>\$config{'zylinder_wp_type'}, -value=>"off", -command=>\&updateCyl);
    $zylmenu->radiobutton(-label => "200m",-variable=>\$config{'zylinder_radius'},-value=>0.2,-command=>\&updateCyl);
    $zylmenu->radiobutton(-label => "300m",-variable=>\$config{'zylinder_radius'},-value=>0.3,-command=>\&updateCyl);
    $zylmenu->radiobutton(-label => "400m",-variable=>\$config{'zylinder_radius'},-value=>0.4,-command=>\&updateCyl);
    $zylmenu->radiobutton(-label => "500m",-variable=>\$config{'zylinder_radius'},-value=>0.5,-command=>\&updateCyl);
    $zylmenu->radiobutton(-label => "1km - DMSt start/finish", -variable=>\$config{'zylinder_radius'},-value=>1.0,-command=>\&updateCyl);
    $zylmenu->checkbutton(-label => "Show names", -variable=>\$config{'zylinder_names'}, -command=>sub{updateCyl(); baroplot();});#_task();});

    $zoommenu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"WP-Plot/Zoom sidelength", -menu=> $zoommenu);

    #$menu_options->separator();
    #$menu_options->command(-label=>'WP-Plot/Zoom sidelength');
    $zoommenu->radiobutton(-label=>"1km",-variable=>\$config{'zoom_sidelength'},-value=>"1",-command=>\&updateZoom);
    $zoommenu->radiobutton(-label=>"3km",-variable=>\$config{'zoom_sidelength'},-value=>"3",-command=>\&updateZoom);
    $zoommenu->radiobutton(-label=>"5km",-variable=>\$config{'zoom_sidelength'},-value=>"5",-command=>\&updateZoom);
    $zoommenu->radiobutton(-label=>"10km",-variable=>\$config{'zoom_sidelength'},-value=>"10",-command=>\&updateZoom);

    $noiselevelmenu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"Noise Level Limit", -menu=> $noiselevelmenu);

    $noiselevelmenu->radiobutton(-label=>"50", -variable=>\$config{'ENL_noise_limit'},-value=>"50",-command=>\&updateFVW);
    $noiselevelmenu->radiobutton(-label=>"100",-variable=>\$config{'ENL_noise_limit'},-value=>"100",-command=>\&updateFVW);
    $noiselevelmenu->radiobutton(-label=>"250",-variable=>\$config{'ENL_noise_limit'},-value=>"250",-command=>\&updateFVW);
    $noiselevelmenu->radiobutton(-label=>"500",-variable=>\$config{'ENL_noise_limit'},-value=>"500",-command=>\&updateFVW);
    $noiselevelmenu->radiobutton(-label=>"750",-variable=>\$config{'ENL_noise_limit'},-value=>"750",-command=>\&updateFVW);

    $datfile="3d.dat"; ###default

    $coormenu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"Coordinate format", -menu=> $coormenu);

    #$menu_options->separator();
    #$menu_options->command(-label=>'Coordinates in');
    $coormenu->radiobutton(-label=>"mm.mmm", -variable=>\$config{'coordinate_print_format'},-value=>"igch",-command=>\&updateCoor);
    $coormenu->radiobutton(-label=>"mm ss", -variable=>\$config{'coordinate_print_format'},-value=>"zanh",-command=>\&updateCoor);
    $coormenu->radiobutton(-label=>"dd.ddddd", -variable=>\$config{'coordinate_print_format'},-value=>"deg",-command=>\&updateCoor);

    # SPEED UNITS
    $speed_menu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"Speed units", -menu=> $speed_menu);
    $speed_menu->radiobutton(-label=>"km/h", -variable=>\$config{'speed_unit_name'},-value=>"km/h",-command=>sub{
            $config{'speed_unit_factor'}=1.0;
            FVWausg();
            FSupdate();
          });
    $speed_menu->radiobutton(-label=>"Knots", -variable=>\$config{'speed_unit_name'},-value=>"knots",-command=>sub{
            $config{'speed_unit_factor'}=0.539956803;
            FVWausg();
            FSupdate();
          });
    $speed_menu->radiobutton(-label=>"Miles per hour", -variable=>\$config{'speed_unit_name'},-value=>"mph",-command=>sub{
            $config{'speed_unit_factor'}=0.621504;
            FVWausg();
            FSupdate();
          });

    $speed_menu->radiobutton(-label=>"m/s", -variable=>\$config{'speed_unit_name'},-value=>"m/s",-command=>sub{
            $config{'speed_unit_factor'}=0.277777777;
            FVWausg();
            FSupdate();
          });

    # VERTICAL SPEED UNITS
    $vspeed_menu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"Vertical speed units", -menu=> $vspeed_menu);
    $vspeed_menu->radiobutton(-label=>"m/s", -variable=>\$config{'vertical_speed_unit_name'},-value=>"m/s",-command=>sub{
            $config{'vertical_speed_unit_factor'}=1.0;
            FVWausg();
          });
    $vspeed_menu->radiobutton(-label=>"Knots", -variable=>\$config{'vertical_speed_unit_name'},-value=>"knots",-command=>sub{
            $config{'vertical_speed_unit_factor'}=1.943844492;
            FVWausg();
          });
    $vspeed_menu->radiobutton(-label=>"ft/min", -variable=>\$config{'vertical_speed_unit_name'},-value=>"ft/min",-command=>sub{
            $config{'vertical_speed_unit_factor'}=196.8;
            FVWausg();
          });
    $vspeed_menu->radiobutton(-label=>"km/h", -variable=>\$config{'vertical_speed_unit_name'},-value=>"km/h",-command=>sub{
            $config{'vertical_speed_unit_factor'}=3.6;
            FVWausg();
          });

    # ALTITUDE UNITS
    $alt_menu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"Altitude units", -menu=> $alt_menu);
    $alt_menu->radiobutton(-label=>"m", -variable=>\$config{'altitude_unit_name'},-value=>"m",-command=>sub{
            $config{'altitude_unit_factor'}=1.0;
            FVWausg();
            baroplot();
          });
    $alt_menu->radiobutton(-label=>"ft", -variable=>\$config{'altitude_unit_name'},-value=>"ft",-command=>sub{
            $config{'altitude_unit_factor'}=3.28;
            FVWausg();
            baroplot();
          });

    # DISTANCE UNITS
    $dist_menu = $menu_options->cget(-menu)->Menu();
    $menu_options->cascade(-label=>"Distance units", -menu=> $dist_menu);
    $dist_menu->radiobutton(-label=>"km", -variable=>\$config{'distance_unit_name'},-value=>"km",-command=>sub{
            $config{'distance_unit_factor'}=1.0;
            FVWausg();
            FSupdate();
          });
    $dist_menu->radiobutton(-label=>"Miles", -variable=>\$config{'distance_unit_name'},-value=>"miles",-command=>sub{
            $config{'distance_unit_factor'}=0.621504;
            FVWausg();
            FSupdate();
          });
    $dist_menu->radiobutton(-label=>"Nautical Miles", -variable=>\$config{'distance_unit_name'},-value=>"NM",-command=>sub{
            $config{'distance_unit_factor'}=0.539956803;
            FVWausg();
            FSupdate();
          });

    $menu_options->separator();
    $menu_options->checkbutton(-label =>"Show accuracy", -variable=>\$config{'draw_accuracy'},-command=>sub{
            if (Exists($FlightView)) {FVWausg();}

          });

    $menu_options->checkbutton(-label => "Photo locator", -variable=>\$config{'photos'},-command=>sub{

            if ($config{'photos'}) {
                checkPhotos();
                processPhotos();
            }

            if (!$config{'photos'}) {
                if (Exists($FlightView)) {
                    trackplot($dxp, $dyp, $minlat, $minlon);
                }
            }
          });
    $menu_options->checkbutton(-label => "Debugging output", -variable=>\$config{'DEBUG'});
    $menu_options->command(-label=>"Save configuration", -command=>\&save_config);
    $menu_options->command(-label=>"Reread configuration", -command=>\&load_config);

    $menu_options->pack(-side=>"left");

###########################################
    # CREATE AND pack Plotting menus!
    my $menu_plot2d=$menuleiste->Menubutton(-text=>"Plots-2D");#, -underline=>0);

    $menu_plot2d->command(-label=>"Barogramm", -command=>sub{$com1="set ylabel \"Altitude [$config{'altitude_unit_name'}]\" \n set xlabel \"time \" \n plot";$com2="\'${pfadftmp}baro.dat\' using (\$1):(\$2*$config{'altitude_unit_factor'}) title \"Barogramm\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Altitude histogram (baro)", -command=>sub{$com1="set nokey\n set ylabel \"% of flighttime\" \n set xlabel \"Altitude (baro) [$config{'altitude_unit_name'}]\" \n plot";$com2="\'${pfadftmp}barohisto.dat\' using (\$1*$config{'altitude_unit_factor'}):(\$2) with boxes fs solid 1 "; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Baro / GPS",-command=>sub{$com1="set ylabel \"Altitude [$config{'altitude_unit_name'}]\" \n set xlabel \"time \" \n plot";$com2="\'${pfadftmp}baro.dat\' using (\$1):(\$2*$config{'altitude_unit_factor'}) title \"Barogramm\""." ".$config{'gnuplot_draw_style'}.", \'${pfadftmp}gpsalt.dat\' using (\$1):(\$2*$config{'altitude_unit_factor'}) title \"GPS Altitude\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Pressure",-command=>sub{$com1="set key top center\nset ylabel \"Pressure [hPa] (QNH $gpi{'qnh'})\" \n set xlabel \"time \" \n f(x)=696.86\ng(x)=631.87\nh(x)=595.29\ni(x)=506.06\nj(x)=376.07\nplot";$com2="\'${pfadftmp}press.dat\' using (\$1):(\$2) title \"Pressure plot\""." ".$config{'gnuplot_draw_style'}.",f(x) title \"FL100\", g(x) title \"FL125\", h(x) title \"FL140\", i(x) title \"FL180\", j(x) title \"FL250\""; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Variogramm (baro)",-command=>sub{$com1="set ylabel \"Vertical speed (baro) [$config{'vertical_speed_unit_name'}]\" \n set xlabel \"time \" \n plot";$com2="\'${pfadftmp}vario.dat\' using (\$1):(\$2*$config{'vertical_speed_unit_factor'})title \"Variogramm (baro)\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Vario histogram (baro)", -command=>sub{$com1="set nokey\n set ylabel \"% of flighttime\" \n set xlabel \"vertical speed (baro) [$config{'vertical_speed_unit_name'}]\" \n plot";$com2="\'${pfadftmp}vhisto.dat\' using (\$1*$config{'vertical_speed_unit_factor'}):(\$2) with boxes fs solid 1 "; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Speedogramm",-command=>sub{$com1="set ylabel \"(10 fixes mean) ground speed [$config{'speed_unit_name'}]\" \n set xlabel \"time \" \n plot";$com2="\'${pfadftmp}speed.dat\' using (\$1):(\$2*$config{'speed_unit_factor'}) title \"Speedogramm\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Indicated air speed",-command=>sub{$com1="set ylabel \"Indicated Airspeed [$config{'speed_unit_name'}]\" \n set xlabel \"time \" \n plot";$com2="\'${pfadftmp}ias.dat\' using (\$1):(\$2*$config{'speed_unit_factor'}) title \"Indicated Airspeed\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"True air speed",-command=>sub{$com1="set ylabel \"True Airspeed [$config{'speed_unit_name'}]\" \n set xlabel \"time \" \n plot";$com2="\'${pfadftmp}tas.dat\' using (\$1):(\$2*$config{'speed_unit_factor'}) title \"True Airspeed\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Engine noise level / RPM",-command=>sub{$com1="set ylabel \"Noise Level / RPM\" \n set xlabel \"time \" \n plot";
            $com2="\'${pfadftmp}enl.dat\' title \"Engine noise level\" ".$config{'gnuplot_draw_style'}.", \'${pfadftmp}enl.dat\' using 1:3 title \"RPM\" ".$config{'gnuplot_draw_style'};
            gnuplot("2d");});

    $menu_plot2d->command(-label=>"Satellites used",-command=>sub{$com1="set ylabel \"Satellites used\" \n set xlabel \"time \" \n plot";$com2="\'${pfadftmp}sat.dat\' title \"Satellites used\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Projection lat (gps)",-command=>sub{$com1="set xlabel \"Latitude []\" \n set ylabel \"Altitude (gps) [$config{'altitude_unit_name'}] \" \n plot";$com2="\'${pfadftmp}latproj.dat\' using (\$1):(\$3*$config{'altitude_unit_factor'}) title \"Flightpath\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Projection lon (gps)",-command=>sub{$com1="set xlabel \"Longitude []\" \n set ylabel \"Altitude (gps) [$config{'altitude_unit_name'}] \" \n plot";$com2="\'${pfadftmp}lonproj.dat\' using (\$1):(\$3*$config{'altitude_unit_factor'}) title \"Flightpath\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"GPS altitude",-command=>sub{
            $com1="set ylabel \"Altitude [$config{'altitude_unit_name'}]\" \n set xlabel \"time \" \n plot";
            $com2="\'${pfadftmp}gpsalt.dat\' using (\$1):(\$2*$config{'altitude_unit_factor'}) title \"GPS Altitude\" ".$config{'gnuplot_draw_style'};
            $com2.=", \'${pfadftmp}gpsalt.dat\' using (\$1):(\$2*$config{'altitude_unit_factor'}):(\$3*$config{'altitude_unit_factor'}) title \"Accuracy\" with yerrorbars" if ($VXAEXISTS eq "yes" && $config{'draw_accuracy'});
            gnuplot("2d");
          });

    $menu_plot2d->command(-label=>"Altitude histogram (gps)", -command=>sub{$com1="set nokey\n set ylabel \"% of flighttime\" \n set xlabel \"Altitude (gps) [$config{'altitude_unit_name'}]\" \n plot";$com2="\'${pfadftmp}gpshisto.dat\' using (\$1*$config{'altitude_unit_factor'}):(\$2) with boxes fs solid 1 "; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Baro-GPS",-command=>sub{$com1="set ylabel \"Difference [$config{'altitude_unit_name'}]\" \n set xlabel \"time \" \n plot";$com2="\'${pfadftmp}deltah.dat\' using (\$1):(\$2*$config{'altitude_unit_factor'}) title \"Difference (Baro-GPS)\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Variogramm (gps)",-command=>sub{$com1="set ylabel \"Vertical speed (gps) [$config{'vertical_speed_unit_name'}]\" \n set xlabel \"time \" \n plot";$com2="\'${pfadftmp}gpsvario.dat\' using (\$1):(\$2*$config{'vertical_speed_unit_factor'})title \"Variogramm (gps)\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Vario histogram (gps)", -command=>sub{$com1="set nokey\n set ylabel \"% of flighttime\" \n set xlabel \"vertical speed (gps) [$config{'vertical_speed_unit_name'}]\" \n plot";$com2="\'${pfadftmp}vhistogps.dat\' using (\$1*$config{'vertical_speed_unit_factor'}):(\$2) with boxes fs solid 1 "; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Groundspeed histogram", -command=>sub{$com1="set nokey\n set ylabel \"% of flighttime\" \n set xlabel \"ground speed [$config{'speed_unit_name'}]\" \n plot";$com2="\'${pfadftmp}shisto.dat\' using (\$1*$config{'speed_unit_factor'}):(\$2) with boxes fs solid 1 "; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Indicated airspeed histogram", -command=>sub{$com1="set nokey\n set ylabel \"% of flighttime\" \n set xlabel \"Indicated Airspeed [$config{'speed_unit_name'}]\" \n plot";$com2="\'${pfadftmp}iashisto.dat\' using (\$1*$config{'speed_unit_factor'}):(\$2) with boxes fs solid 1 "; gnuplot("2d");});

    $menu_plot2d->command(-label=>"True airspeed histogram", -command=>sub{$com1="set nokey\n set ylabel \"% of flighttime\" \n set xlabel \"True Airspeed [$config{'speed_unit_name'}]\" \n plot";$com2="\'${pfadftmp}tashisto.dat\' using (\$1*$config{'speed_unit_factor'}):(\$2) with boxes fs solid 1 "; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Accuracy",-command=>sub{$com1="set ylabel \"Position Accuracy [$config{'altitude_unit_name'}]\" \n set xlabel \"time \" \n plot";
            $com2="\'${pfadftmp}sat.dat\' using 1:(\$3*$config{'altitude_unit_factor'}) title \"Horizontal Accuracy\" $config{'gnuplot_draw_style'}, \'${pfadftmp}sat.dat\' using 1:(\$4*$config{'altitude_unit_factor'}) title \"Vertical Accuracy\" $config{'gnuplot_draw_style'} "; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Track plot",-command=>sub{$com1="set ylabel \"Latitude []\" \n set xlabel \"Longitude [] \" \n plot";$com2="\'$pfadftmp$datfile\' title \"Flightpath\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Projection lat (baro)",-command=>sub{$com1="set xlabel \"Latitude []\" \n set ylabel \"Altitude (baro) [$config{'altitude_unit_name'}] \" \n plot";$com2="\'${pfadftmp}latproj.dat\' using (\$1):(\$2*$config{'altitude_unit_factor'}) title \"Flightpath\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    $menu_plot2d->command(-label=>"Projection lon (baro)",-command=>sub{$com1="set xlabel \"Longitude []\" \n set ylabel \"Altitude (baro) [$config{'altitude_unit_name'}] \" \n plot";$com2="\'${pfadftmp}lonproj.dat\' using (\$1):(\$2*$config{'altitude_unit_factor'}) title \"Flightpath\" ".$config{'gnuplot_draw_style'}; gnuplot("2d");});

    #$menuleiste->pack(-side=>'top', -fill=>'x',-padx=>3);
    $menu_plot2d->pack(-side=>'left');

###########################################
    # CREATE AND pack Plotting menus!
    my $menu_plot3d=$menuleiste->Menubutton(-text=>"Plots-3D");#, -underline=>0);

    $menu_plot3d->command(-label=>"3D View (baro)",-command=>sub{$com1="set ylabel \"Latitude []\" \n set xlabel \"Longitude []\" \n set zlabel \"Altitude (baro)[$config{'altitude_unit_name'}]\" \n splot";$com2="\'$pfadftmp$datfile\' using (\$1):(\$2):(\$3*$config{'altitude_unit_factor'}) title \"Flightpath\" ".$config{'gnuplot_draw_style'}; gnuplot("3d");});
    $menu_plot3d->command(-label=>"3D View (gps)",-command=>sub{$com1="set ylabel \"Latitude []\" \n set xlabel \"Longitude []\" \n set zlabel \"Altitude (gps)[$config{'altitude_unit_name'}]\" \n splot";$com2="\'$pfadftmp$datfile\' using (\$1):(\$2):(\$4*$config{'altitude_unit_factor'}) title \"Flightpath\" ".$config{'gnuplot_draw_style'}; gnuplot("3d");});
    $menu_plot3d->command(-label=>"3D View + groundtrack (baro)",-command=>sub{$com1="set ylabel \"Latitude []\" \n set xlabel \"Longitude []\" \n set zlabel \"Altitude (baro)[$config{'altitude_unit_name'}]\" \n splot";$com2="\'$pfadftmp$datfile\' using (\$1):(\$2):(\$3*$config{'altitude_unit_factor'}) title \"Flightpath\""." ".$config{'gnuplot_draw_style'}.", \'$pfadftmp$datfile\' using (\$1):(\$2):(\$3*0) title \"Groundtrack\" ".$config{'gnuplot_draw_style'}; gnuplot("3d");});
    $menu_plot3d->command(-label=>"3D View + groundtrack (gps)",-command=>sub{$com1="set ylabel \"Latitude []\" \n set xlabel \"Longitude []\" \n set zlabel \"Altitude (gps)[$config{'altitude_unit_name'}]\" \n splot";$com2="\'$pfadftmp$datfile\' using (\$1):(\$2):(\$4*$config{'altitude_unit_factor'}) title \"Flightpath\""." ".$config{'gnuplot_draw_style'}.", \'$pfadftmp$datfile\' using (\$1):(\$2):(\$4*0) title \"Groundtrack\" ".$config{'gnuplot_draw_style'}; gnuplot("3d");});

    $menu_plot3d->command(-label=>"Clear ranges",-command=>sub{$xmin='';$xmax='';$ymin='';$ymax='';$zmin='0';$zmax='';});

    #$menuleiste->pack(-side=>'top', -fill=>'x',-padx=>3);
    $menu_plot3d->pack(-side=>'left');

    my $menu_stuff=$menuleiste->Menubutton(-text=>"Tools");#, -underline=>0);
### frame fuer restl funktionen
    $menu_stuff->command(-label=>"Flight info (igc)",-command=>\&FlightInfo);

    $menu_stuff->command(-label=>"Flight info (additional)",-command=>\&gpiinput);

    $menu_stuff->command(-label=>"Flight statistics",-command=>\&FlightStatistics);

    $menu_stuff->command(-label=>"Task editor",-command=>\&WpPlot);

    $menu_stuff->command(-label=>"OGIE - 3d", -command=>\&OpenGLexplorer); #sub{system("ogie ${pfadftmp}km.dat");});

    $menu_stuff->pack(-side=>'left');

    $zoom = 0;

    $fvwtext="\n\n\n\n";  # five lines

    my $top_text = $FlightView->Label(-textvariable=>\$fvwtext)->pack(-side=>"top");


    # looks better with solid 1 relief
    $canvas_frame = $FlightView->Frame(-borderwidth=>0);  #-width=>$config{'window_width'},-borderwidth=>0);


    $trackheight = int ((     (1-(1/$config{'fvw_baro_fraction'}))*$config{'window_height'}    )+0.5)-2;

    # for some UNKNOWN reason the canvas is gonna be two pixels wider than requested !!!!!!!!!!!!!!!!!!
    $canvas=$canvas_frame->Canvas(-height=>$trackheight, -width=>$config{'window_width'}-2, -background=>"white",-borderwidth=>0,
        -scrollregion=>[0,0,$config{'window_width'}-2,$trackheight]);

    $canvas->CanvasBind('<1>' => \&viewclick);
    $canvas->CanvasBind('<B3-Motion>' => \&zoomdrag);
    $canvas->CanvasBind('<3>' => \&zoompress);
    $canvas->CanvasBind('<B3-ButtonRelease>' => \&zoomrelease);
    $canvas->pack(-expand=>"yes",-fill=>"both");

    # Here we add a slim strip for the barograph canvas...
    $baroheight = int ( ($config{'window_height'}/ $config{'fvw_baro_fraction'})+0.5 )-2;
    $barocanvas = $canvas_frame->Canvas(-height=>$baroheight, -width=>$config{'window_width'}-2, -background=>"grey90",-borderwidth=>0,
        -scrollregion=>[0,0,$config{'window_width'}-2,$baroheight]);

    $barocanvas->pack(-expand=>"yes",-fill=>"both");
    $barocanvas->CanvasBind('<B1-Motion>' => \&baroclick);
    $barocanvas->CanvasBind('<1>' => \&baroclick);

    $canvas_frame->pack(-expand=>"yes",-fill=>"both");

    # box to determine fine pixel borders
    @debugboxlist=();
    if ($config{'DEBUG'}){
    push(@debugboxlist, $canvas->createLine(0,$trackheight-1,$config{'window_width'}-3,$trackheight-1 ,-fill=>"red"));
    push(@debugboxlist, $canvas->createLine($config{'window_width'}-3,0,$config{'window_width'}-3, $trackheight, -fill=>"red"));
    push(@debugboxlist, $canvas->createLine(0,0,0,$trackheight,-fill=>"red"));
    push(@debugboxlist, $canvas->createLine(0,0,$config{'window_width'}-3,0, -fill=>"red"));
    }

    if ($config{'DEBUG'}){
    push(@debugboxlist, $barocanvas->createLine(0,$baroheight-1,$config{'window_width'}-3,$baroheight-1 ,-fill=>"green"));
    push(@debugboxlist, $barocanvas->createLine($config{'window_width'}-3,0,$config{'window_width'}-3, $baroheight-1, -fill=>"green"));
    push(@debugboxlist, $barocanvas->createLine(0,0,0,$baroheight-1,-fill=>"green"));
    push(@debugboxlist, $barocanvas->createLine(0,0,$config{'window_width'}-3,0, -fill=>"green"));
    }

    $nr=0;

    unzoomed();
    mapplot($dxp, $dyp, $minlat, $minlon,"i");

    trackplot($dxp, $dyp, $minlat, $minlon);
    baroplot();

    #$fvwtextobj=$canvas->createText(0,0,text=>" ");
    $horizontal=$canvas->createLine(0,0,0,0);
    $vertical=$canvas->createLine(0,0,0,0);
    $barovertical = $barocanvas->createLine(0,0,0,0);

    FVWausg();
    marksdraw();

    $FlightView->bind("<Key-F3>", sub {if ($filestat eq 'closed') { return; } $nr++ ; FVWausg();});
    $FlightView->bind("<Key-F4>", sub {if ($filestat eq 'closed') { return; } $nr=$nr+5 ; FVWausg();});
    $FlightView->bind("<Key-F1>", sub {if ($filestat eq 'closed') { return; } $nr=$nr-5 ; FVWausg();});
    $FlightView->bind("<Key-F2>", sub {if ($filestat eq 'closed') { return; } $nr-- ; FVWausg();});
    $FlightView->bind("<Key-F5>", sub {
	    if ($filestat eq 'closed') { return; }
	    $stat_start=$nr;$FlightView->bell; baroplot();
            if ($cut_track == 1) {trackplot($dxp, $dyp, $minlat, $minlon);FVWausg();}
            marksdraw();

          });

    # reset F5/F6 not documented
    $FlightView->bind("<Key-F12>", sub {
	    if ($filestat eq 'closed') { return; }
	    $stat_start=0;$stat_end=$#BARO; $FlightView->bell;
            marksdraw();
            $cut_track=0;trackplot($dxp, $dyp, $minlat, $minlon);baroplot();FVWausg();
          });
    $FlightView->bind("<Key-F6>", sub {
	    if ($filestat eq 'closed') { return; }
	    $stat_end=$nr;$FlightView->bell; baroplot();
            if ($cut_track == 1) {trackplot($dxp, $dyp, $minlat, $minlon);FVWausg();}
            marksdraw();
          });
    $FlightView->bind("<Key-F7>", sub {if ($filestat eq 'closed') { return; }statistik($stat_start, $stat_end);});
    $FlightView->bind("<Key-F8>", sub {if ($filestat eq 'closed') { return; }thermik_statistik();});
    $FlightView->bind("<Key-F9>", sub {if ($filestat eq 'closed') { return; }glide_statistik();});

    $FlightView->bind("<Key-F10>", sub {
	    if ($filestat eq 'closed') { return; }
            $FlightView->bell;
            my $lifts = substr($file, 0, -3)."lif";
            $liftsexist =1;
            open (LIFTS, ">>$lifts");

            # vielleicht noch das datum!
            my ($name, $p, $suf) = File::Basename::fileparse($file);
            printf LIFTS ("%.4f %.4f %.0f %.2f %.2f %.3f %02d-%02d-%04d $name\n",$DECLAT[$nr],$DECLON[$nr],$BARO[$nr],$IVARIO[$nr],$nonIVARIO[$nr],$DECTIME[$nr],$gpi{'day'},$gpi{'month'},$gpi{'year'});
            close LIFTS;

          });

    $FlightView->bind("<Key-F11>", sub {
	    if ($filestat eq 'closed') { return; }
            if ($cut_track == 0) {$cut_track=1;trackplot($dxp, $dyp, $minlat, $minlon);baroplot();FVWausg();return;}
            if ($cut_track == 1) {$cut_track=0;trackplot($dxp, $dyp, $minlat, $minlon);baroplot();FVWausg();}
          });

    $FlightView->bind("<Key-s>", sub {
	    if ($filestat eq 'closed') { return; }
            if ($nr < $task_finish_time_index) {
                $task_start_time_index = $nr; $FlightView->bell;
                FSupdate();
                baroplot();

                #baroplot_task(); in baroplot already
                marksdraw();

            #if ($task_state ==1) {taskdraw(1);}
            #if task changed since optimization loose ability to olc-file output
                undef $olcFirstWPindex;
                $optimized_for = "none";
            }
          });

    $FlightView->bind("<Key-f>", sub {
	    if ($filestat eq 'closed') { return; }
            if ($nr > $task_start_time_index) {
                $task_finish_time_index = $nr; $FlightView->bell;
                FSupdate();
                baroplot();

                #baroplot_task();
                marksdraw();

            #if ($task_state ==1) {taskdraw(1);}
            #if task changed since optimization loose ability to olc-file output
                undef $olcFirstWPindex;
                $optimized_for = "none";
            }
          });

    $FlightView->bind("<Key-e>",sub {if ($filestat eq 'closed') { return; } calibrationinput();});
    $FlightView->bind("<Key-n>",sub {if ($filestat eq 'closed') { return; } qnhcalibrationinput();});
    $FlightView->bind("<Key-x>",sub {if ($filestat eq 'closed') { return; } phototimecalibration();});
    $FlightView->bind("<Key-u>",sub {if ($filestat eq 'closed') { return; } geotag_photos();});
    $FlightView->bind("<Key-m>",sub {
	    if ($filestat eq 'closed') { return; }

            # sub medialist
            my $list;
            if ($#PHOTO_FILES < 1 ) {Errorbox("List is empty"); return;}
            if (Exists($mmlist)) {$mmlist->destroy;}

            $mmlist=$FlightView->Toplevel();
            $mmlist->configure(-title=>"Media List");
            setexit($mmlist);

            $mm_list_box = $mmlist->ScrlListbox(-label=>"Media List", -selectmode=>"single",-height=>"0",-width=>"0")->pack(-side=>"left");
            $mm_list_box->bind("<Double-Button-1>" => \&mm_click);

            for (my $z=0; $z<=$#PHOTO_FILES;$z++) {
                $list = GPLIGCfunctions::dec2time($PHOTO_FILES_TIMES[$z])." $PHOTO_FILES[$z]";
                $mm_list_box->insert("end", $list);
            };
            sub mm_click {
                my $sel=$mm_list_box->curselection();

                if (defined @$sel[0]) {
                    FVWausg($PHOTO_FILES_INDICES[@$sel[0]]);
                }

            }
          });

    $FlightView->bind("<Key-r>", sub{
      if ($filestat eq 'closed') { return; }
      Ausschnitt($DECLAT[$nr], $DECLON[$nr]); $FlightView->bell;
    });

    $FlightView->bind("<Key-t>", sub{
	    if ($filestat eq 'closed') { return; }
            if ($task_state == 0) {taskdraw(1);$task_state=1;return;}
            if ($task_state == 1) {taskdraw(0);$task_state=0;}
          });

    $FlightView->bind("<Key-T>", sub{
	    if ($filestat eq 'closed') { return; }
	    unzoomed(1);
	    if ($task_state == 1) {taskdraw(0);}
            if ($wpcyl_state == 1) {wpcyldraw(0);}
            mapplot($dxp, $dyp, $minlat, $minlon,"z");
            trackplot($dxp, $dyp, $minlat, $minlon);
            if ($task_state == 1) {taskdraw(1);}
            if ($wpcyl_state == 1) {wpcyldraw(1);}
            marksdraw();
            FVWausg();
	    $zoom=0;
          });

    $FlightView->bind("<Key-w>", sub{
	    if ($filestat eq 'closed') { return; }
            $FlightView->bell;
            push(@WPNAME, "WP$freeWP");
            $freeWP++;
            $MAXWP++;
            push(@WPLAT, $DECLAT[$nr]);
            push(@WPLON, $DECLON[$nr]);
            push(@WPRADFAC, 1.0);

            WPPlotUpdate();
            baroplot(); #_task();
            FSupdate();
            TaskUpdate();

            #if task changed since optimization loose ability to olc-file output
            undef $olcFirstWPindex;
            $optimized_for = "none";
          });

    $FlightView->bind("<Key-a>", sub{
	    if ($filestat eq 'closed') { return; }
            $FlightView->bell;
            print "Bef: aktwp: $AKTWP + 1, maxwp $MAXWP, nowplat: $#WPLAT \n" if ($config{'DEBUG'});

	    my $add = 1;
	    if ($AKTWP == 1 && $MAXWP == 0) {$add = 0;}

            splice(@WPNAME, $AKTWP+$add , 0 , "WP$freeWP");
            splice(@WPLAT, $AKTWP+$add , 0 , $DECLAT[$nr]);
            splice(@WPLON, $AKTWP+$add , 0 , $DECLON[$nr]);
            splice(@WPRADFAC, $AKTWP+$add , 0 , 1.0);
            $freeWP++;
            $MAXWP++;

	    print "Aft: aktwp: $AKTWP + 1, maxwp $MAXWP, nowplat: $#WPLAT \n" if ($config{'DEBUG'});

            WPPlotUpdate();
            baroplot();#_task();
            FSupdate();
            TaskUpdate();
            FVWausg();

            #if task changed since optimization loose ability to olc-file output
            undef $olcFirstWPindex;
            $optimized_for = "none";
          });

    $FlightView->bind("<Key-b>", sub{
	    if ($filestat eq 'closed') { return; }
	    if ($AKTWP == 1 && $MAXWP == 0) {return;} #no other WPs yet
            $FlightView->bell;
            splice(@WPNAME, $AKTWP , 0 , "WP$freeWP");
            splice(@WPLAT, $AKTWP , 0 , $DECLAT[$nr]);
            splice(@WPLON, $AKTWP , 0 , $DECLON[$nr]);
            splice(@WPRADFAC, $AKTWP , 0 , 1.0);
            $freeWP++;
            $MAXWP++;

            $AKTWP++;
            WPPlotUpdate();
            baroplot();#_task();
            FSupdate();

            FVWausg();
            TaskUpdate();

            #if task changed since optimization loose ability to olc-file output
            undef $olcFirstWPindex;
            $optimized_for = "none";
          });

    $FlightView->bind("<Key-p>", sub{
	    if ($filestat eq 'closed') { return; }
	    if ($AKTWP == 1 && $MAXWP == 0) {return;} #no other WPs yet
            $FlightView->bell;
            splice(@WPNAME, $AKTWP , 1 , "WP$freeWP");
            splice(@WPLAT, $AKTWP , 1 , $DECLAT[$nr]);
            splice(@WPLON, $AKTWP , 1 , $DECLON[$nr]);
            splice(@WPRADFAC, $AKTWP , 1 , 1.0);
            $freeWP++;

            WPPlotUpdate();
            baroplot();#_task();
            FSupdate();
            FVWausg();
            TaskUpdate();

            #if task changed since optimization loose ability to olc-file output
            # replace should be allowed, in case modification is needed...
            #undef $olcFirstWPindex;
          });

    $FlightView->bind("<Key-g>", sub{
	    if ($filestat eq 'closed') { return; }

            if ($config{'fvw_grid'} eq 'yes' && $config{'fvw_baro_grid'} eq 'yes') {
                $config{'fvw_grid'} = 'no' ;  $config{'fvw_baro_grid'} = 'no';
                updateFVW(); return;
            }

            if ($config{'fvw_grid'} eq 'no' && $config{'fvw_baro_grid'} eq 'no') {
                $config{'fvw_grid'} = 'yes' ;  $config{'fvw_baro_grid'} = 'no';
                updateFVW(); return;
            }

            if ($config{'fvw_grid'} eq 'yes' && $config{'fvw_baro_grid'} eq 'no') {
                $config{'fvw_grid'} = 'no' ;  $config{'fvw_baro_grid'} = 'yes';
                updateFVW(); return;
            }

            if ($config{'fvw_grid'} eq 'no' && $config{'fvw_baro_grid'} eq 'yes') {
                $config{'fvw_grid'} = 'yes' ;  $config{'fvw_baro_grid'} = 'yes';
                updateFVW(); return;
            }
          });

    $FlightView->bind("<Key-o>", sub{
	    if ($filestat eq 'closed') { return; }
            $savename = save();
            if ($savename eq "no") {return;}
            MessageFenster("Postscript Output", "$savename wird ausgegeben");
            $canvas->postscript(-file=>$savename, -colormode=>"color");
          });

    $FlightView->bind("<Key-i>", sub{
	    if ($filestat eq 'closed') { return; }
            $savename = save();
            if ($savename eq "no") {return;}
            MessageFenster("Postscript Output", "$savename wird ausgegeben");
            $barocanvas->postscript(-file=>$savename, -colormode=>"color");
          });

    $FlightView->bind("<Key-h>", sub{
	    if ($filestat eq 'closed') { return; }
            if ($hide_mm == 0) {$hide_mm =1;} else {$hide_mm=0;}
            updateFVW();
          });

    $FlightView->bind('<Key-y>',sub{ if ($filestat eq 'closed') { return; } FVWresize();});
    $FlightView->bind("<Key-z>", sub{ if ($filestat eq 'closed') { return; } zoom();});
    $FlightView->bind("<Key-v>", sub{
	if ($filestat eq 'closed') { return; }
            # view next photo
            my $min = 100000;
            my $min_idx = -77;
            for (my $z=0; $z<=$#PHOTO_FILES_INDICES; $z++){
                if ( abs($nr-$PHOTO_FILES_INDICES[$z]) < $min ) {
                    $min = abs($nr-$PHOTO_FILES_INDICES[$z]);
                    $min_idx = $z;
                }
            }

            if ($min_idx != -77) {
                $lastphoto_idx = $min_idx;
                print "The last photo: $lastphoto_idx  $PHOTO_FILES[$lastphoto_idx] \n$PHOTO_FILES_TIMES[$lastphoto_idx]  $PHOTO_FILES_INDICES[$lastphoto_idx]\n" if ($config{'DEBUG'});
                showMM($PHOTO_FILES[$min_idx]);
            }
          });

    # Turn on the following by default...
    $task_state=$config{'draw_task'};
    $wpcyl_state=$config{'draw_wpcyl'};

    taskdraw($task_state);
    wpcyldraw($wpcyl_state);

    $FlightView->bind("<Key-c>",sub {
      if ($filestat eq 'closed') { return; }
            # hier kommt die wendepunktzeichenroutine rein!
            if ($wpcyl_state == 0) {wpcyldraw(1);$wpcyl_state=1;return;}
            if ($wpcyl_state == 1) {wpcyldraw(0);$wpcyl_state=0;}
          });

    $FlightView->bind("<plus>", sub{
      if ($filestat eq 'closed') { return; }
            if ($config{'maps'}) {
                if ($zl < 18) {
                    $zl++;
                    updateFVW(); #FVWausg();
                }
            }
          } );

    $FlightView->bind("<Key-M>", sub{
      if ($filestat eq 'closed') { return; }
            if ($config{'maps'}) {$config{'maps'}=0;}else{$config{'maps'}=1;}

            updateFVW();
            #mapplot($dxp, $dyp, $minlat, $minlon,"z");
            #trackplot($dxp, $dyp, $minlat, $minlon);
            #marksdraw();
            #baroplot();
            #FVWausg();

          } );

    $FlightView->bind("<minus>", sub{
      if ($filestat eq 'closed') { return; }
            if ($config{'maps'}) {
                if ($zl > 0) {
                    $zl--;
                    updateFVW(); #FVWausg();
                }
            }
          } );

    # hash-key "#" trigger re-download
    $FlightView->bind("<numbersign>", sub{
	if ($filestat eq 'closed') { return; }
            if ($config{'maps'}) {
                $map_reload = 1;
                updateFVW(); #FVWausg();

            }
          } );


    $FlightView->bind("<Key-H>", sub{
      print "$config{'fvw_grid'}=fvw_grid   $config{'fvw_baro_grid'}=fvw_baro_grid\n";

      print @baroplot ." bp\n";
      print @baroplot_task ." bpt\n";
      print @trackplot ." tp\n";
      print @mapplot ." mp\n";
      print @task_lines ." tl\n";
      print @acc_lines ." accl\n";
      print @marks_lines ." ml\n";
      print @wpcyl_lines ." wpcl\n";

      # ZZZZ
      #this is purely for debugging one memory leak
      # VERY STRANGE
      # symptoms:
      # repeatedly pressing H leaks some memory
      # ONLY if fvw_grid is "no" ???? WTF
      # fvw_grid isnt used at ALL in baroplot()
      #

      baroplot();

    } );

    $FlightView->bind("<Escape>", sub{$FlightView->destroy; gpligcexit();}  );
}

sub mapplot {
    if ($filestat eq 'closed') { return; }

    print "mapplot ---------\n" if ($config{'DEBUG'});

    if ($config{'maps'} && !($have_imager && $have_png)) {
        $config{'maps'}=0;
        Errorbox("You cant use maps without PNG and Imager modules!");
        return;
    }

    if (!($have_imager && $have_png)) {return;}

    # maps are off? then delete them (if present) and return
    if (!$config{'maps'}) {
	if (@mapplot) { $canvas->delete(@mapplot); @mapplot=();}
        return;
    }

    # check or load needed modules
    require Tk::PNG;
    use File::Path;
    require Imager;
    use MIME::Base64;# qw(decode_base64);
    require LWP::Simple;

    if ($config{'DEBUG'}) {
        print "mapplot: zoomlevel=$zl\n";
    }

    # maxima,minima: do nothing:
    # ZZZ this should be selective for different map-types!
    if ($zl > 18) {$zl=18; return;}
    if ($zl < 0 ) {$zl=0; return;}
    if ($config{'DEBUG'}) {print "FIXME! mapplot, zoomlevel-checks (min/max)\n";}

    # remember number of map tiles
    my $number_of_tiles_old=$tiles;

    $FlightView->Busy;

    # set up path
    my $osmpath=$config{'map_path'}."/".$config{'map_type'};

    # for hybrid layer we need two passes.
    if ($config{'map_type'} eq "gmh") {$osmpath=$config{'map_path'}."/gms";}
    my $addlayer = 0; #additional layer. this will be set to 1 in second pass

    # get resolution and minima from func call
    my ($dxp, $dyp, $minlat, $minlon, $calledfrom) =@_;

    # calc maxima
    my $maxlat = $minlat + $dyp * ($trackheight-1);
    my $maxlon = $minlon + $dxp * ($config{'window_width'}-1);

    # jump address for reload at changed zl (if zl has to be changed because of limits)
    mapreload:

    # calc upper left and lower right tile at this zl
    my ($x1,$y1) = GPLIGCfunctions::getTileNumber($maxlat,$minlon,$zl);
    my ($x2,$y2) = GPLIGCfunctions::getTileNumber($minlat,$maxlon,$zl);

    $tiles = ($x2-$x1+1)*($y2-$y1+1);

    if ($tiles > $config{'map_max_tiles'}) {
        $zl-- if ($zl >0);
        if ($calledfrom eq "u") {
            $tiles=$number_of_tiles_old;
            return;
        }
        goto mapreload;
    }

    print "mapplot: needed tiles: $x1,$y1 => $x2,$y2\n" if ($config{'DEBUG'});
    my $xt=$x1;
    my $yt=$y1;

    # check for maximum scaling before entering the loop! prevents uneccessary reloading of the map
    my ($tlatmin,$tlonmin,$tlatmax,$tlonmax) =GPLIGCfunctions::Project( $xt,$yt,$zl);

    # calculate resizing of tile (they ship as 256x256)
    my $xsize = ($tlonmax-$tlonmin)/$dxp; #print "xsise $xsize\n";
    my $ysize = ($tlatmax-$tlatmin)/$dyp; #print "ysize $ysize\n";

    if ($config{'DEBUG'}) {print "mapplot scalesize $xsize\n";}

    if ($xsize > $config{'map_max_scalesize'}) {
        $zl++;
        if ($zl > 18) {$zl = 18;};
        if ($calledfrom eq "u") {
            $tiles=$number_of_tiles_old;
            return;
        }

        if ($zl == 18 && $xsize > $config{'map_max_scalesize'} ) {
            print "mapplot: accepting large scaling. Very small track?\n" if ($config{'DEBUG'});

        } else {
            goto mapreload;
        }
    }

    if (@mapplot) {$canvas -> delete (@mapplot); @mapplot=();}  # delete old maps

    # we'll jump here for the second pass (hybrid layer)
    maplayer:

    # loop to load and display map tiles in range
    while ($xt<=$x2 && $yt<=$y2) {

        # get boundaries for actual map tile
        my ($tlatmin,$tlonmin,$tlatmax,$tlonmax) =GPLIGCfunctions::Project( $xt,$yt,$zl);

        # calculate resizing of tile (they ship as 256x256)
        my $xsize = ($tlonmax-$tlonmin)/$dxp; #print "xsise $xsize\n";
        my $ysize = ($tlatmax-$tlatmin)/$dyp; #print "ysize $ysize\n";

        # check if tile exists and re-download  (not for openflightmap, which is installed locally)
        if ( (!-e "$osmpath/$zl/$xt/$yt.png" || $map_reload == 1) && !($config{'map_type'} eq "ofm-b" || $config{'map_type'} eq "ofm-a" ||  $config{'map_type'} eq "ofm-m" )){

            $canvas -> delete (@textplot);
            @textplot=();
            my $dlplottext="DOWNLOADING MAP TILE ($config{'map_type'} $zl $xt $yt";


            $dlplottext .= ")";
            push (@textplot, $canvas->createText(20,20, -anchor=>"w", -text=>$dlplottext, -fill=>"red"));
            $canvas->update;

            my $dladdr;
            $dladdr = "http://tile.openstreetmap.org/$zl/$xt/$yt.png" if ($config{'map_type'} eq "osm");
            $dladdr = "http://a.tile.opencyclemap.org/cycle/$zl/$xt/$yt.png" if ($config{'map_type'} eq "osmC");

            # seems not to work, yet?
            $dladdr = "http://t1.openseamap.org/seamark/$zl/$xt/$yt.png" if ($config{'map_type'} eq "oseam");

            $rand = int(rand(4));
            $dladdr = "http://mt$rand.google.com/vt/lyrs=t,r&hl=en&x=$xt&y=$yt&z=$zl" if ($config{'map_type'} eq "gmt");
            $dladdr = "http://mt$rand.google.com/vt/lyrs=m&hl=en&x=$xt&y=$yt&z=$zl" if ($config{'map_type'} eq "gmm" );

            $retry_gms_counter=-1;
            retry_gms:
            $retry_gms_counter++;

            if ($retry_gms_counter >= 50) {
                Errorbox("cannot download map tile. (20x404). giving up. Maps disabled");
                $config{'maps'}=0;
                $canvas -> delete (@textplot);
                goto load_map_tiles;
            }

	    # for sat and hybrid load sat layer

            # new 2019! seems like the satellite imagery comes from mt{n}. with lyrs=s, without version number now.
            $dladdr = "http://mt$rand.google.com/vt/lyrs=s&hl=en&x=$xt&y=$yt&z=$zl" if ($config{'map_type'} eq "gms" || $config{'map_type'} eq "gmh" );

            $dladdr = "http://mt$rand.google.com/vt/lyrs=h&hl=en&x=$xt&y=$yt&z=$zl" if ($addlayer == 1);

            if (!-d "$osmpath/$zl/$xt") {File::Path::mkpath "$osmpath/$zl/$xt";}
            $resp = LWP::Simple::getstore($dladdr, "$osmpath/$zl/$xt/$yt.png");
            print "$resp DOWNLOAD: \"$dladdr\" ==> $osmpath/$zl/$xt/$yt.png\n" if ($config{'DEBUG'});

            if ($resp != 200) {print "$resp error: $dladdr\n" if ($config{'DEBUG'}); }

          # not needed yet. evtl. for later use by ogie?! solange er kein png versteht
          # system ("convert $osmpath/$zl/$xt/$yt.png $osmpath/$zl/$xt/$yt.jpg");
        }

        # load image and resize
        # check for zero size before!
        load_map_tiles:
        if (-s "$osmpath/$zl/$xt/$yt.png") {

            $canvas -> delete (@textplot);
            @textplot=();
            push (@textplot, $canvas->createText(20,20, -anchor=>"w", -text=>"LOADING MAP TILE ($config{'map_type'} $zl $xt $yt)", -fill=>"black"));
            $canvas->update;

            my $img = Imager->new;
            $img->read(file => "$osmpath/$zl/$xt/$yt.png");
            my $newimg = $img->scale(xpixels=>$xsize+.5, ypixels=>$ysize+.5);

            #my $zzz;
            #$newimg->write(data => \$zzz, type=>"png");  #OLD Method, mem-leak!
            $newimg->write(file => "${pfadftmp}tmp.png" , type=>"png"); # now, via tmp file

            # this creates a horrible memory leak. see:
            # https://rt.cpan.org/Public/Bug/Display.html?id=29640
            # http://www.perlmonks.org/?node_id=986015
            #my $nmt= $FlightView->Photo(-data => encode_base64($zzz));
            my $nmt= $FlightView->Photo(-file => "${pfadftmp}tmp.png");

            # new code, does NOT work
            #if (!defined $nmt) {
	    #  $nmt = $FlightView->Photo();
            #}
            #$nmt->configure(-data => encode_base64($zzz));

            # calculate shift
            my $cx = ($tlonmin - $minlon)/$dxp ;
            my $cy =  ($tlatmax - $maxlat)/$dyp;


            # load into canvas
            push(@mapplot, $canvas->createImage($cx,-$cy, -anchor=>"nw", -image => $nmt));

            #$nmt->delete;


        } else {

            # file has 0 bytes:
            print "ERROR $osmpath/$zl/$xt/$yt.png has size 0! probably failed download. I'll delete this.\n" if ($config{'DEBUG'});
            unlink ("$osmpath/$zl/$xt/$yt.png");

        }
        $canvas -> delete (@textplot);
        @textplot=();

        # next tiles!
        $xt++;
        if ($xt>$x2) {$xt=$x1;$yt++;}
    }

    # add second layer for gm hybrid
    if ($addlayer == 0 && $config{'map_type'} eq "gmh") {
        $addlayer=1;
        $osmpath=$config{'map_path'}."/gmh";
        $xt=$x1;
        $yt=$y1;
        goto maplayer;
    }

#######################################
    # draw boxes for debugging:
    #	if ($config{'DEBUG'}) {
    #	    $xt=$x1;
    #	    $yt=$y1;
    #	    while ($xt<=$x2 && $yt<=$y2) {

# get boundaries for actual map tile
#	      my ($tlatmin,$tlonmin,$tlatmax,$tlonmax) =GPLIGCfunctions::Project( $xt,$yt,$zl);
#print "Tile $xt,$yt: latmin:$tlatmin, lonmin:$tlonmin, latmax:$tlatmax, lonmax:$tlonmax\n";

    # calculate resizing of tile (they ship as 256x256)
    #	      my $xsize = ($tlonmax-$tlonmin)/$dxp; #print "xsise $xsize\n";
    #	      my $ysize = ($tlatmax-$tlatmin)/$dyp; #print "ysize $ysize\n";

    #	      my $cx = ($tlonmin - $minlon)/$dxp ;
    #	      my $cy =  ($tlatmax - $maxlat)/$dyp;

#	      push(@mapplot, $canvas->createLine($cx,-$cy,                       $cx+$xsize+.5, -$cy , -fill=>"black"));
#	      push(@mapplot, $canvas->createLine($cx,-$cy,                       $cx, -$cy+$ysize+0.5, -fill=>"black"));
#	      push(@mapplot, $canvas->createLine($cx+$xsize+.5,-$cy+$ysize+0.5,  $cx+$xsize+.5, -$cy, -fill=>"black"));
#	      push(@mapplot, $canvas->createLine($cx+$xsize+.5,-$cy+$ysize+0.5,  $cx, -$cy+$ysize+.5, -fill=>"black"));

    #	      $xt++;
    #	      if ($xt>$x2) {$xt=$x1;$yt++;}
    #	    }
    #	}
#######################################


    $FlightView->Unbusy;
    $canvas -> delete (@textplot);
    @textplot=();
    $map_reload = 0;
}

########### FlightView end #########################

sub trackplot {
    if ($filestat eq 'closed') {
        return;
    }

    print "trackplot ---------\n" if ($config{'DEBUG'});

    $FlightView->Busy;
    my ($dxp, $dyp, $minlat, $minlon) =@_;

    print "TRACKPLOT (dxp dyp minlat minlon): $dxp  $dyp  $minlat  $minlon\n" if ($config{'DEBUG'});

    if (@trackplot) {$canvas -> delete (@trackplot);}
    @trackplot =();

    # gridlines to be added here
    if ($config{'fvw_grid'} eq 'yes') {

        my $gridstep= 1/6;

        #first lat grid line at....
        my $firstlatgrid =  int($minlat) + int(($minlat - int($minlat)) / $gridstep)* $gridstep;

        for (my $latgrid = $firstlatgrid; $trackheight > (($latgrid-$minlat)/$dyp); $latgrid += $gridstep) {

            #print "$latgrid ...\n";
            #print " ".abs($latgrid-int($latgrid))."      \n";

            my $y = int ($trackheight - (($latgrid-$minlat)/$dyp));

            #bei ganzen graden....
            if (abs($latgrid-int($latgrid)) < 0.01 ||  abs($latgrid-int($latgrid)) > 0.99   ) {
                push (@trackplot, $canvas->createLine(0, $y, $config{'window_width'}-2, $y,-fill=>'gray70'));
                my $text = int ($latgrid + 0.5)."";
                push (@trackplot, $canvas->createText(0,$y, -anchor=>"w", -text=>$text, -fill=>"gray30"));
            } else {
                push (@trackplot, $canvas->createLine(0, $y, $config{'window_width'}-2, $y,-fill=>'gray90'));
            }
        }

        #first lon grid line at....
        my $firstlongrid =  int($minlon) + int(($minlon - int($minlon)) / $gridstep)* $gridstep;

        for (my $longrid = $firstlongrid; $config{'window_width'}-2 > (($longrid-$minlon)/$dxp); $longrid += $gridstep) {

            #print "$latgrid ...\n";

            my $x = int (($longrid-$minlon)/$dxp);

            if (abs($longrid-int($longrid)) < 0.01 ||  abs($longrid-int($longrid)) > 0.99   ) {

                push (@trackplot, $canvas->createLine($x, 0, $x, $trackheight,-fill=>"gray80"));
                my $text =int ($longrid + 0.5)."";
                push (@trackplot, $canvas->createText($x, 0, -anchor=>"n", -text=>$text, -fill=>"black"));

            } else {
                push (@trackplot, $canvas->createLine($x, 0, $x, $trackheight,-fill=>"gray90"));
            }
        }
    }

    # TRACKPLOT
    # This plots the Track

    my $pstart = 1;
    my $pend=$#LAT;
    if ($cut_track == 1 && !($stat_start == 0 && $stat_end == $#BARO) && $stat_start < $stat_end) {
        $pstart=$stat_start+1; $pend=$stat_end;
    }

    for (my $zaehler=$pstart; $zaehler<=$pend ;$zaehler++) {
        my $x1= int(($DECLON[$zaehler-1]-$minlon)/$dxp);
        my $x2= int(($DECLON[$zaehler]-$minlon)/$dxp);
        my $y1= int($trackheight-(($DECLAT[$zaehler-1]-$minlat)/$dyp));
        my $y2= int($trackheight-(($DECLAT[$zaehler]-$minlat)/$dyp));

        if ($ENL[$zaehler] < $config{'ENL_noise_limit'}) {
            if ($nonIVARIO[$zaehler] <= 0) {push (@trackplot, $canvas->createLine( $x1, $y1, $x2, $y2,-fill=>"blue"));}
            if ($nonIVARIO[$zaehler] > 0) {push (@trackplot, $canvas->createLine( $x1, $y1, $x2, $y2,-fill=>"red"));}

            # display skips in different color
            if ($config{'skip_check'}) {
                if ($DECTIME[$zaehler] - $DECTIME[$zaehler-1] > ($config{'skip_limit_minutes'}/60)) {
                    push (@trackplot, $canvas->createLine( $x1, $y1, $x2, $y2,-fill=>"black",-width=>2,-arrow=>"both"));
                    print "skip detected at >>".($zaehler-1)." - $zaehler \n" if ($config{'DEBUG'});
                }

            }

            # This is powered flight!!!!
        } else {
            push (@trackplot, $canvas->createLine( $x1, $y1, $x2, $y2,-fill=>"green"));
        }

        if ($config{'photos'} && !$hide_mm) {
            for (my $g = 0; $g <= $#PHOTO_FILES; $g++) {
                if ($PHOTO_FILES_INDICES[$g] == $zaehler-1) {
                    my ($name, $p, $suf) = File::Basename::fileparse($PHOTO_FILES[$g]);

                    #print "$name <<<<<<<<<<\n";

                    push(@trackplot, $canvas->createText($x1, $y1, -anchor=>"w", -text=>$name,-fill=>"gray10"));
                }
            }
        }
    }
    $FlightView->Unbusy;
}

sub wpcyldraw {
    my $on_off= shift;

    print "wpcyldraw (on/off = $on_off) ---------\n" if ($config{'DEBUG'});

    if ($on_off == 1) {
        if ($MAXWP >= 1) {
            @wpcyl_lines=();
            for (my $zaehl=1; $zaehl<=$#WPLAT; $zaehl++){

                # draw cylinders
                if ($config{'zylinder_wp_type'} eq "cyl" || $config{'zylinder_wp_type'} eq "both") {
                    my ($cylref_lat, $cylref_lon) = GPLIGCfunctions::zylinder2( $WPLAT[$zaehl], $WPLON[$zaehl], $config{'zylinder_radius'}*($WPRADFAC[$zaehl]+0.000000000001), 0, 359.99999);

                    # adding a very small value in the last line seems to circumvent some bug in perl?
                    # without it the result of zyl_rad * wprfac comes as integer wtf????

#                    if ($config{'DEBUG'}) {
#                     my $GGG1= $WPRADFAC[$zaehl] * $config{'zylinder_radius'};
#                     my $GGG2= ($WPRADFAC[$zaehl]+0.000000000001)* $config{'zylinder_radius'};

#                     if (abs($GGG1-$GGG2) > 0.1) {
#                     print "IS THIS A BUG in Perl ???:\n";
#                     print 'This is the value of the hash/key >>> $config{\'zylinder_radius\'} :>>'.$config{'zylinder_radius'}."<<\n";
#                     print 'This is the value of the array/index >>> $WPRADFAC[$zaehl] :>>'.$WPRADFAC[$zaehl]."<<\n";
#                     print 'The product $WPRADFAC[$zaehl] * $config{\'zylinder_radius\'} = '.$GGG1."\n";
#                     print 'The product ($WPRADFAC[$zaehl]+0.000000000001) * $config{\'zylinder_radius\'} = '.$GGG2."\n";
#                     print "\n*******************\n  $GGG1 = $GGG2 \n*******************\n\n";
#                     }
#                    }

                    for  (my $xzaehl=0; $xzaehl<=$#$cylref_lat-1; $xzaehl++) {

                        my $x1= int( ( ($WPLON[$zaehl]+${$cylref_lon}[$xzaehl]) -$minlon) /$dxp);
                        my $x2= int( ( ($WPLON[$zaehl]+${$cylref_lon}[$xzaehl+1]) -$minlon) /$dxp);
                        my $y1= int($trackheight-( ( ($WPLAT[$zaehl]+${$cylref_lat}[$xzaehl])   -$minlat )/$dyp));
                        my $y2= int($trackheight-( ( ($WPLAT[$zaehl]+${$cylref_lat}[$xzaehl+1]) -$minlat )/$dyp));
                        push(@wpcyl_lines, $canvas->createLine( $x1, $y1, $x2, $y2,-width=>$config{'cylinder_linewidth'},-fill=>"dark violet"));
                    }
                }

                if ($config{'zylinder_names'} == 1) {

                    my $y = int($trackheight-(($WPLAT[$zaehl]-$minlat) / $dyp )+ 0.5) ;
                    my $x = int((($WPLON[$zaehl]-$minlon) / $dxp )+ 0.5) ;

                    push(@wpcyl_lines, $canvas->createText($x,$y,-anchor=>"n",-text=>$WPNAME[$zaehl],-fill=>"gray10"));

                }

                if ($config{'zylinder_wp_type'} eq "sec" || $config{'zylinder_wp_type'} eq "both") {
                    if ($MAXWP > 1) {
                        my $secdir = 0;

                        # in this cases it is the first or last wp!
                        if ($zaehl == 1) { $secdir = GPLIGCfunctions::gegenkurs(GPLIGCfunctions::kurs( $WPLAT[$zaehl], $WPLON[$zaehl], $WPLAT[$zaehl+1], $WPLON[$zaehl+1])); }
                        if ($zaehl == $#WPLAT) { $secdir = GPLIGCfunctions::gegenkurs(GPLIGCfunctions::kurs( $WPLAT[$zaehl], $WPLON[$zaehl], $WPLAT[$zaehl-1], $WPLON[$zaehl-1])); }

                        #a "normal wp"
                        if ($zaehl != 1 && $zaehl != $#WPLAT ) { $secdir = GPLIGCfunctions::sectordirection(
                                GPLIGCfunctions::kurs( $WPLAT[$zaehl], $WPLON[$zaehl], 	$WPLAT[$zaehl+1], $WPLON[$zaehl+1]),
                                GPLIGCfunctions::kurs( $WPLAT[$zaehl], $WPLON[$zaehl], 	$WPLAT[$zaehl-1], $WPLON[$zaehl-1])); }

                        $sec2up = $secdir +90;  if ($sec2up < 0) { $sec2up = $sec2up + 360 ;} if ($sec2up > 360) { $sec2up = $sec2up - 360 ;}
                        $sec2down = $secdir -90;  if ($sec2down < 0) { $sec2down = $sec2down + 360 ;} if ($sec2down > 360) { $sec2down = $sec2down - 360 ;}
                        $sec1up = $secdir +45;  if ($sec1up < 0) { $sec1up = $sec1up + 360 ;} if ($sec1up > 360) { $sec1up = $sec1up - 360 ;}
                        $sec1down = $secdir -45;  if ($sec1down < 0) { $sec1down = $sec1down + 360 ;} if ($sec1down > 360) { $sec1down = $sec1down - 360 ;}

                        ($cylref_lat, $cylref_lon) = GPLIGCfunctions::zylinder2($WPLAT[$zaehl], $WPLON[$zaehl], '3.0' , $sec2down, 180);  # FAI = 3.0 km sector

                        for  (my $xzaehl=0; $xzaehl<=$#$cylref_lat-1; $xzaehl++) {

                            my $x1= int( ( ($WPLON[$zaehl]+${$cylref_lon}[$xzaehl]) -$minlon) /$dxp);
                            my $x2= int( ( ($WPLON[$zaehl]+${$cylref_lon}[$xzaehl+1]) -$minlon) /$dxp);
                            my $y1= int($trackheight-( ( ($WPLAT[$zaehl]+${$cylref_lat}[$xzaehl])   -$minlat )/$dyp));
                            my $y2= int($trackheight-( ( ($WPLAT[$zaehl]+${$cylref_lat}[$xzaehl+1]) -$minlat )/$dyp));

                            if ($zaehl == 1) {
                                push(@wpcyl_lines, $canvas->createLine( $x1, $y1, $x2, $y2,-width=>$config{'waypoint_linewidth'} ,-fill=>"dark red"));
                            } else {
                                push(@wpcyl_lines, $canvas->createLine( $x1, $y1, $x2, $y2,-width=>$config{'waypoint_linewidth'} ,-fill=>"dark green"));
                            }
                        }

                        my $whatline=0; # used to make sec2up/down for first wp 10km (starting-line)
                        foreach ($sec2up, $sec2down, $sec1up, $sec1down) {

                            #print "teil: $_ \n";

                            my $x1= int( ( ($WPLON[$zaehl]) -$minlon) /$dxp);
                            my $y1= int($trackheight-( ( ($WPLAT[$zaehl])   -$minlat )/$dyp));

                            my $base_line = 3;
                            if ($zaehl == 1 && ($whatline == 0 || $whatline==1)) {$base_line=$config{'starting_line'}/2.0;}
                            $whatline++;
                            my ($lat, $lon) = GPLIGCfunctions::go($WPLAT[$zaehl],$WPLON[$zaehl], $_, $base_line);

                            #print " $lat   $lon \n";

                            my $x2= int( ( ($lon) -$minlon) /$dxp);
                            my $y2= int($trackheight-( ( ($lat)   -$minlat )/$dyp));

                            if ($zaehl == 1) {
                                push(@wpcyl_lines, $canvas->createLine( $x1, $y1, $x2, $y2,-width=>$config{'waypoint_linewidth'},-fill=>"dark red"));
                            } else {
                                push(@wpcyl_lines, $canvas->createLine( $x1, $y1, $x2, $y2,-width=>$config{'waypoint_linewidth'},-fill=>"dark green"));
                            }

                        }

                    }
                }
            }
        }
    }

    if ($on_off == 0){
        $canvas->delete(@wpcyl_lines);
        @wpcyl_lines=();
    }
}

sub taskdraw {
    my $on_off = shift;

    print "taskdraw (on/off = $on_off) ---------\n" if ($config{'DEBUG'});

    if ($on_off == 1) {

        # delete task-lines and replot
        $canvas->delete(@task_lines);
        @task_lines=();
        for (my $zaehl=1 ; $zaehl<=$#WPLAT-1; $zaehl++) {
            my $x1= int(($WPLON[$zaehl]-$minlon)/$dxp);
            my $x2= int(($WPLON[$zaehl+1]-$minlon)/$dxp);
            my $y1= int($trackheight-(($WPLAT[$zaehl]-$minlat)/$dyp));
            my $y2= int($trackheight-(($WPLAT[$zaehl+1]-$minlat)/$dyp));
            push(@task_lines, $canvas->createLine( $x1, $y1, $x2, $y2,-width=>$config{'task_linewidth'},-fill=>"dark violet"));
        }
    }

    if ($on_off == 0) {
        $canvas->delete(@task_lines);
        @task_lines=();
    }
}


sub baroplot {
    if ($filestat eq 'closed') {
        return;
    }

    print "baroplot ---------\n" if ($config{'DEBUG'});

        # erase all plotted stuff from previous and clean array!
    if (@baroplot){$barocanvas -> delete (@baroplot);}
    @baroplot = (0,1); undef @baroplot;
    @baroplot = ();

    if (@baroplot_task){$barocanvas -> delete (@baroplot_task);}
    @baroplot_task=();

    $FlightView->Busy;

    # JMWK 2.9.2001    heavily edited by kruegerh october 2002

    my $baronumber = $#BARO;
    my $bstart = 0;
    my $bend = $#BARO-1;
    my $bcut = 0;
    my $bmaxbaro = $maxbaro;
    my $bminbaro = $minbaro;

    if ($cut_track == 1 && !($stat_start == 0 && $stat_end == $#BARO) && $stat_end > $stat_start) {
        $baronumber = $stat_end - $stat_start;
        $bstart=$stat_start; $bend=$stat_end;
        $bcut=1;
        $bminbaro = 99999;
        $bmaxbaro = -99999;
        for (my $x=$stat_start; $x <= $stat_end; $x++) {
            $bminbaro = $BARO[$x] if ($bminbaro > $BARO[$x]);
            $bmaxbaro = $BARO[$x] if ($bmaxbaro < $BARO[$x]);
        }
    }

    $dxbaro = $baronumber/($config{'window_width'}-3);
    $dybaro = (($bmaxbaro * $config{'altitude_unit_factor'}) - ($bminbaro*$config{'altitude_unit_factor'}))/($baroheight-1);

    # dev by zero workaround (for files all altitudes are 0)
    if ($dybaro == 0) {
        $dybaro = 0.1 ;
        Errorbox("No altitude information found...");
    }


    # plot area of unpowered flight (white background)
    my $x1 = int ( ($task_start_time_index-$bstart) / $dxbaro);
    my $y1 = int (($baroheight-1)-((($bmaxbaro*$config{'altitude_unit_factor'})-($bminbaro*$config{'altitude_unit_factor'}))/$dybaro));


    my $x2 = int ( ($task_finish_time_index-$bstart) / $dxbaro);
    my $y2 = int (($baroheight-1)-((($bmaxbaro*$config{'altitude_unit_factor'})-($bminbaro*$config{'altitude_unit_factor'}))/$dybaro));

    push (@baroplot, $barocanvas->createRectangle($x1,$y1,$x2,$baroheight,-fill=>"white",-outline=>"white"));

      # plot selected statistics zone (red background)
      # this needs to be done before "start" and "finish" text
      if ( !($stat_start == 0 && $stat_end == $#BARO) && $bcut != 1) {
	  my $rx1 = int ($stat_start / $dxbaro);
	  my $ry1 = int ($baroheight-((($bmaxbaro*$config{'altitude_unit_factor'})-($bminbaro*$config{'altitude_unit_factor'}))/$dybaro));
	  my $rx2 = int ($stat_end / $dxbaro);
	  my $ry2 = int ($baroheight);
	  push (@baroplot, $barocanvas->createRectangle($rx1,$ry1,$rx2,$ry2,-fill=>"#FFEEEE",-outline=>"#FFEEEE"));
      }

    push (@baroplot_task, $barocanvas->createText($x1, $y1,-anchor=>"n",-text=>"start" , -fill=>"black"));
    push (@baroplot_task, $barocanvas->createText($x2, $y2,-anchor=>"n",-text=>"finish" , -fill=>"black"));

    if ($config{'fvw_baro_grid'} eq 'yes') {

        # horizontal lines (500, 1000m) in barogram
        for (my $z = int($minbaro*$config{'altitude_unit_factor'}) ; $z <= ($maxbaro*$config{'altitude_unit_factor'}); $z++) {

            if ( ($z % $config{'baro_grid_small'}) == 0 ) {

                my $x1 = int ( 1 / $dxbaro );
                my $x2 = int ( $#BARO / $dxbaro );
                my $y = int ($baroheight- (($z-($bminbaro*$config{'altitude_unit_factor'}))/$dybaro));

                if ( ($z % $config{'baro_grid_large'}) == 0) {

                    push (@baroplot, $barocanvas->createLine($x1, $y, $x2, $y, -fill=>"gray80"));
                    push (@baroplot, $barocanvas->createText($x1, $y,-anchor=>"w",-text=>$z , -fill=>"black"));

                } else {

                    push (@baroplot, $barocanvas->createLine($x1, $y, $x2, $y, -fill=>"gray90"));
                }
            }
        }

        # vertical lines every hour
        for (my $zaehler = 2; $zaehler <$#DECTIME; $zaehler++) {

            if ( int($DECTIME[$zaehler]) != int($DECTIME[$zaehler+1]) ) {

                my $zeit = int($DECTIME[$zaehler+1]);
                while ($zeit >= 24) {$zeit-=24;}
                my $x = int ( ($zaehler-$bstart) / $dxbaro);
                my $y1 = int ($baroheight);
                my $y2 = int ($baroheight-((($bmaxbaro*$config{'altitude_unit_factor'})-($bminbaro*$config{'altitude_unit_factor'}))/$dybaro));

                push (@baroplot, $barocanvas->createLine($x, $y1, $x, $y2, -fill=>"gray80"));
                push (@baroplot, $barocanvas->createText($x, $y1,-anchor=>"s",-text=>$zeit , -fill=>"gray30"));
            }
        }
    }

    baroplot_task();

    #barogram-plotting
    for (my $zaehler=$bstart; $zaehler<$bend ;$zaehler++) {
        my $x1= int((($zaehler-$bstart)/$dxbaro)+.5);
        my $x2= int(((($zaehler-$bstart)+1)/$dxbaro)+.5);
        my $y1= int((($baroheight-1)-((($BARO[$zaehler]*$config{'altitude_unit_factor'})-($bminbaro*$config{'altitude_unit_factor'}))/$dybaro))+.5);
        my $y2= int((($baroheight-1)-((($BARO[$zaehler+1]*$config{'altitude_unit_factor'})-($bminbaro*$config{'altitude_unit_factor'}))/$dybaro))+.5);

        if ($ENL[$zaehler] < $config{'ENL_noise_limit'}) {
            if ($nonIVARIO[$zaehler] <= 0) {push (@baroplot, $barocanvas->createLine( $x1, $y1, $x2, $y2,-fill=>"blue"));}
            if ($nonIVARIO[$zaehler] > 0) {push (@baroplot, $barocanvas->createLine( $x1, $y1, $x2, $y2,-fill=>"red"));}

            # display skips in different color
            if ($config{'skip_check'}) {
                if ($DECTIME[$zaehler] - $DECTIME[$zaehler-1] > ($config{'skip_limit_minutes'}/60)) {

                    my $y1 = int ($baroheight);
                    my $y2 = int ($baroheight-((($bmaxbaro*$config{'altitude_unit_factor'})-($bminbaro*$config{'altitude_unit_factor'}))/$dybaro));
                    push (@baroplot, $barocanvas->createLine( $x1, $y1, $x1, $y2,-fill=>"black",-width=>2,-arrow=>"both"));

#print "skip detected at >>".($zaehler-1)." - $zaehler \n" if ($config{'DEBUG'});
                }

            }

        } else {
            push (@baroplot, $barocanvas->createLine( $x1, $y1, $x2, $y2,-fill=>"green"));
        }
    }

    $FlightView->Unbusy;
}


# will be called from baroplot only
sub baroplot_task {
    if (!Exists($FlightView)) {return;}
#    if (@baroplot_task){$barocanvas -> delete (@baroplot_task);}
#    @baroplot_task =();

    print "baroplot_task ---------\n" if ($config{'DEBUG'});

    my ($wpindicesref, $wpnamesref) = getWPreachedIndices();

    # DISPLAY lines in barogram, where wp were reached
    my $counter = 0;
    my $add = 0;

    if ($cut_track == 1 && !($stat_start == 0 && $stat_end == $#BARO) && $stat_end > $stat_start) {
        $add = $stat_start;
    }

    foreach (@{$wpindicesref}) {

        my $x = int ( ($_-$add) / $dxbaro);
        my $y1 = int ($baroheight);
        my $y2 = int ($baroheight-((($maxbaro*$config{'altitude_unit_factor'})-($minbaro*$config{'altitude_unit_factor'}))/$dybaro)) ;

        #print "NEU\n $_\nx=$x\ny1=$y1\ny2=$y2\n";

        push (@baroplot_task, $barocanvas->createLine($x, $y1, $x, $y2, -fill=>"black"));

        if ($config{'zylinder_names'} ==1 ) {
            push (@baroplot_task, $barocanvas->createText($x, $y2,-anchor=>"n",-text=>$wpnamesref->[$counter++] , -fill=>"gray30"));
        }

    }

}

# draw marks (start/end of unpowered flight, F5/F6)
sub marksdraw {

    if ($filestat eq 'closed') {
        return;
    }

    print "marksdraw ---------\n" if ($config{'DEBUG'});

    if (@marks_lines) {$canvas->delete(@marks_lines);}

    @marks_lines=();
    foreach my $z ($task_start_time_index, $task_finish_time_index) {
        my ($cylref_lat, $cylref_lon) = GPLIGCfunctions::zylinder2($DECLAT[$z], $DECLON[$z], '0.1', 0, 359.99999);  # point = 0.1
        for  (my $xzaehl=0; $xzaehl<=$#$cylref_lat-1; $xzaehl++) {
            my $x1= int( ( ($DECLON[$z]+${$cylref_lon}[$xzaehl]) -$minlon) /$dxp);
            my $x2= int( ( ($DECLON[$z]+${$cylref_lon}[$xzaehl+1]) -$minlon) /$dxp);
            my $y1= int($trackheight-( ( ($DECLAT[$z]+${$cylref_lat}[$xzaehl])   -$minlat )/$dyp));
            my $y2= int($trackheight-( ( ($DECLAT[$z]+${$cylref_lat}[$xzaehl+1]) -$minlat )/$dyp));
            push(@marks_lines, $canvas->createLine( $x1, $y1, $x2, $y2,-fill=>"black",-width=>$config{'marker_linewidth'}));
        }
    }

    foreach my $z ($stat_start, $stat_end) {

        if ($z != 0 && $z != 999999) {
            my ($cylref_lat, $cylref_lon) = GPLIGCfunctions::zylinder2($DECLAT[$z], $DECLON[$z], '0.1', 0, 359.99999);
            for  (my $xzaehl=0; $xzaehl<=$#$cylref_lat-1; $xzaehl++) {
                my $x1= int( ( ($DECLON[$z]+${$cylref_lon}[$xzaehl]) -$minlon) /$dxp);
                my $x2= int( ( ($DECLON[$z]+${$cylref_lon}[$xzaehl+1]) -$minlon) /$dxp);
                my $y1= int($trackheight-( ( ($DECLAT[$z]+${$cylref_lat}[$xzaehl])   -$minlat )/$dyp));
                my $y2= int($trackheight-( ( ($DECLAT[$z]+${$cylref_lat}[$xzaehl+1]) -$minlat )/$dyp));
                push(@marks_lines, $canvas->createLine( $x1, $y1, $x2, $y2,-fill=>"green",-width=>$config{'marker_linewidth'}));
            }
        }
    }
}

sub baroclick {
    if ($filestat eq 'closed') { return; }

    my $e = $barocanvas->XEvent;
    my ($barocanv_x, $barocanv_y) = ($e->x, $e->y);

    # (This is how a 'reverse' lookup should usually be done...)
    $nr = int($barocanv_x * $dxbaro);

    # zoomed baro view
    if ($cut_track == 1 && !($stat_start == 0 && $stat_end == $#BARO) && $stat_end > $stat_start) {
        $nr += $stat_start;
    }

    # Disable the calbacks until we're finisched. (fixes bug #6)
    $barocanvas->CanvasBind('<B1-Motion>' , "");
    $barocanvas->CanvasBind('<1>' , "");
    FVWausg();
    $barocanvas->CanvasBind('<B1-Motion>' => \&baroclick);
    $barocanvas->CanvasBind('<1>' => \&baroclick);
}

sub zoom {
    print "zoom()\n" if ($config{'DEBUG'});
    if ($zoom == 1 || $zoom == 2) {
        unzoomed ();
        $zoom=0;
        if ($task_state == 1) {taskdraw(0);}
        if ($wpcyl_state == 1) {wpcyldraw(0);}
        mapplot($dxp, $dyp, $minlat, $minlon,"z");
        trackplot($dxp, $dyp, $minlat, $minlon);
        if ($task_state == 1) {taskdraw(1);}
        if ($wpcyl_state == 1) {wpcyldraw(1);}
        marksdraw();
        FVWausg();
        $zoom = 0;
        return;
    }

    if ($zoom == 0) {
        $zoom=1;
        ($maxlon, $minlon, $maxlat, $minlat)=GPLIGCfunctions::Ausschnitt2($trackheight, $config{'window_width'}-2, $DECLAT[$nr], $DECLON[$nr], $config{'zoom_sidelength'});

        $delta_lat = $maxlat-$minlat;
        if ($delta_lat == 0) {$delta_lat = 0.5;}
        $delta_lon = $maxlon-$minlon;
        if ($delta_lon == 0) {$delta_lon = 0.5;}
        $dyp=($delta_lat)/($trackheight-1);
        $dxp=($delta_lon)/($config{'window_width'}-1);

        if ($task_state == 1) {taskdraw(0);}
        if ($wpcyl_state == 1) {wpcyldraw(0);}
        mapplot($dxp, $dyp, $minlat, $minlon,"z");
        trackplot($dxp, $dyp, $minlat, $minlon);
        if ($task_state == 1) {taskdraw(1);}
        if ($wpcyl_state == 1) {wpcyldraw(1);}
        marksdraw();
        FVWausg();
        $zoom=1;
    }
}

sub unzoomed {
    if ($filestat eq 'closed') {
        return;
    }

    my $trackzoom = 0; # 1 -> unzoom to track boundaries
    if (@_ >= 1) {
      $trackzoom = shift;
    }

    print "\nunzoomed($trackzoom)\n" if ($config{'DEBUG'});

    if (!$trackzoom) {
      ($maxlat, $minlat)=GPLIGCfunctions::MaxKoor(\@DECLAT);
      ($maxlon, $minlon)=GPLIGCfunctions::MaxKoor(\@DECLON);
    } else {
      $WPLAT[0]=$WPLAT[1]; # fox for stupidity
      $WPLON[0]=$WPLON[1]; # fox for stupidity  WPLAT arrays start at 1. elem 0 is otherwise undef
      my @alllat =();
      push (@alllat, @WPLAT);
      push (@alllat, @DECLAT);
      my @alllon =();
      push (@alllon, @WPLON);
      push (@alllon, @DECLON);

      ($maxlat, $minlat)=GPLIGCfunctions::MaxKoor(\@alllat);
      ($maxlon, $minlon)=GPLIGCfunctions::MaxKoor(\@alllon);
    }

    #now we add  $bord km borders, to include all sectors
    my $bordlat = $config{'zoom_border'} * 0.008993216;
    my $bordlon = ($config{'zoom_border'} * 0.008993216) / cos(deg2rad($centrelat));

    $maxlat += $bordlat;
    $minlat -= $bordlat;
    $minlon -= $bordlon;
    $maxlon += $bordlon;

    print "\nunzoomed($trackzoom) $maxlat $minlat $maxlon $minlon \n" if ($config{'DEBUG'});

    $max_kmdist_x = GPLIGCfunctions::dist($centrelat,$minlon,$centrelat,$maxlon);
    $max_kmdist_y = GPLIGCfunctions::dist($minlat,$centrelon,$maxlat,$centrelon);

    $fvw_side_ratio=$trackheight/($config{'window_width'});

    if ($max_kmdist_x eq 0) { print STDERR "max_kmdist_x == 0, forced to 1\n"; $max_kmdist_x=1;}
    if ($max_kmdist_y eq 0) { print STDERR "max_kmdist_y == 0, forced to 1\n"; $max_kmdist_y=1;}

    $task_side_ratio=$max_kmdist_y/$max_kmdist_x;

    # x (lon) is limiting
    if ($task_side_ratio <= $fvw_side_ratio ) {

        $delta_lon = $maxlon-$minlon;
        if ($delta_lon == 0) {$delta_lon = 0.5;}

	# -3 avoids plotting over the edge (rounding)
        $dxp = ($delta_lon)/($config{'window_width'}-3); 	#degrees x per pixel (usually > $dyp)
        $dyp = $dxp * (cos(deg2rad($centrelat))); 		#degrees y per pixel

	# what the heck subtract was good for?!
        my $subtract = $dyp * ($trackheight - (($config{'window_width'}) * $task_side_ratio))/2;

        $minlat = $minlat - $subtract;
        $maxlat = $maxlat + $subtract;

    } else { # y (lat) is limiting

        $delta_lat = $maxlat-$minlat;
        if ($delta_lat == 0) {$delta_lat = 0.5;}

        $dyp = ($delta_lat)/($trackheight-1); 	#degrees y per pixel
        $dxp = $dyp*(1/cos(deg2rad($centrelat))); #degrees x per pixel (usually > $dyp)

        my $subtract = $dxp * (($config{'window_width'}) - (($trackheight)/$task_side_ratio))/2;

        $minlon = $minlon - $subtract;
        $maxlon = $maxlon + $subtract;
    }
}
# end unzoomed


sub zoom2 {
    print "zoom2()\n" if ($config{'DEBUG'});
    #my $recenter = shift;
    if (!defined shift) {
        ($maxlon, $minlon, $maxlat, $minlat)=GPLIGCfunctions::Ausschnitt2($trackheight, $config{'window_width'}-2, $zoomclickcenterx, $zoomclickcentery, $kl2);
    }  else {
        ($maxlon, $minlon, $maxlat, $minlat)=GPLIGCfunctions::Ausschnitt2($trackheight, $config{'window_width'}-2, $DECLAT[$nr], $DECLON[$nr], $kl2);
    }

    $delta_lat = $maxlat-$minlat;
    if ($delta_lat == 0) {$delta_lat = 0.5;}
    $delta_lon = $maxlon-$minlon;
    if ($delta_lon == 0) {$delta_lon = 0.5;}
    $dyp=($delta_lat)/($trackheight-1);
    $dxp=($delta_lon)/($config{'window_width'}-3);

    if ($task_state == 1) {taskdraw(0);}
    if ($wpcyl_state == 1) {wpcyldraw(0);}

    $zoom=2;
    mapplot($dxp, $dyp, $minlat, $minlon,"z");
    trackplot($dxp, $dyp, $minlat, $minlon);

    if ($task_state == 1) {taskdraw(1);}
    if ($wpcyl_state == 1) {wpcyldraw(1);}
    marksdraw();

    FVWausg();

    $zoom=2;
}


sub viewclick {
    if ($filestat eq 'closed') { return; }
    my $e = $canvas->XEvent;
    my ($canvas_x, $canvas_y) = ($e->x, $e->y);

    #print "$canvas_x   $canvas_y \n";

    my $lat_click = ($trackheight - $canvas_y) * $dyp    +  $minlat;
    my $lon_click =  $canvas_x * $dxp + $minlon;

    my $index_of_nearest=1;
    my $minimum_distance=5000;

    for (my $z=0; $z <= $#DECLAT; $z=$z+$config{'viewclick_res'}) {	#increment of 3 is only for saving time (slow machines....)
        my $actual_dist=GPLIGCfunctions::dist($lat_click, $lon_click, $DECLAT[$z], $DECLON[$z]);
        if ($actual_dist < $minimum_distance) {
            $minimum_distance = $actual_dist;
            $index_of_nearest = $z;
        }
    }

    $nr=$index_of_nearest;
    FVWausg($index_of_nearest);
}


sub zoomdrag {
    if ($filestat eq 'closed') { return; }
    my $e = $canvas->XEvent;
    my ($canvas_x, $canvas_y) = ($e->x, $e->y);
    if (@zoomrubber) {$canvas -> delete (@zoomrubber);}
    @zoomrubber=();
    push(@zoomrubber, $canvas->createRectangle($zoompressedX, $zoompressedY, $canvas_x, $canvas_y, -outline=>"black"));

    if ($config{'DEBUG'}) {print "$zoompressedX $zoompressedY $canvas_x $canvas_y\n";}
}


sub zoompress {
    if ($filestat eq 'closed') { return; }
    my $e = $canvas->XEvent;
    my ($canvas_x, $canvas_y) = ($e->x, $e->y);
    $zoompressedX = $canvas_x;
    $zoompressedY = $canvas_y;
}


sub zoomrelease {
    if ($filestat eq 'closed') { return; }
    my $e = $canvas->XEvent;
    my ($canvas_x, $canvas_y) = ($e->x, $e->y);
    if (@zoomrubber){$canvas -> delete (@zoomrubber);}
    if ( (($zoompressedX - $canvas_x) == 0) || (($zoompressedY - $canvas_y) == 0) ) {
        return;
    }

    my $lat_rel = ($trackheight - $canvas_y) * $dyp    +  $minlat;
    my $lon_rel =  $canvas_x * $dxp + $minlon;

    my $lat_click = ($trackheight - $zoompressedY) * $dyp    +  $minlat;
    my $lon_click =  $zoompressedX * $dxp + $minlon;

    my $zoomareasideratio = abs($zoompressedY - $canvas_y) / abs($zoompressedX - $canvas_x);
    my $canvassideratio = $canvas->Height / $canvas->Width;

    if ($config{'DEBUG'}) {
        print "Release: $lat_rel $lon_rel  Click: $lat_click $lon_click\n";
        print "zoomarea $zoomareasideratio: ";
        print "landscape \n" if ($zoomareasideratio <= 1);
        print "portrait\n" if ($zoomareasideratio > 1);
        print "canvas $canvassideratio\n";
    }

    $zoomclickcenterx = ($lat_rel + $lat_click) / 2;
    $zoomclickcentery = ($lon_rel + $lon_click) / 2;

    $kl2 = GPLIGCfunctions::dist($zoomclickcenterx, $lon_click, $zoomclickcenterx, $lon_rel);

    if ($zoomareasideratio <=  $canvassideratio) {
        print "Selection is more landscape than window\n" if ($config{'DEBUG'});
    }

    if ($zoomareasideratio > $canvassideratio) {
        print "Selection is more portrait than window: $kl2\n" if ($config{'DEBUG'});
        $kl2 *= $zoomareasideratio;
        print "$kl2\n" if ($config{'DEBUG'});
        $kl2 /= $canvassideratio;
        print "$kl2\n" if ($config{'DEBUG'});

    }

    ## test, ob pointer in neuem ausschnitt liegt...
    my $TEST = 'OUT';

    my ($maxlonT, $minlonT, $maxlatT, $minlatT)=GPLIGCfunctions::Ausschnitt2($trackheight, $config{'window_width'}-2, $zoomclickcenterx, $zoomclickcentery, $kl2);

    my $dypT=($maxlatT-$minlatT)/($trackheight-1);
    my $dxpT=($maxlonT-$minlonT)/($config{'window_width'}-1);

    my $xT= int(($DECLON[$nr]-$minlonT)/$dxpT);
    my $yT= int($trackheight-(($DECLAT[$nr]-$minlatT)/$dypT));

    if ($xT > 0 && $xT < $config{'window_width'}-1) {
        if ($yT > 0 && $yT < $trackheight) {
            $TEST = 'INSIDE';
        }
    }

    ########
    if ($TEST eq 'OUT') {
        my $index_of_nearest=1;
        my $minimum_distance=5000;

        for (my $z=0; $z <= $#DECLAT; $z=$z+3) {	#increment of 3 is only for saving time (slow machines....)

            my $actual_dist=GPLIGCfunctions::dist($zoomclickcenterx, $zoomclickcentery, $DECLAT[$z], $DECLON[$z]);

            if ($actual_dist < $minimum_distance) {
                $minimum_distance = $actual_dist;
                $index_of_nearest = $z;
            }
        }

        $nr=$index_of_nearest;

        #print "nummer $nr \n";
    }

    zoom2();
}

sub FVWresize {

    if ($config{'DEBUG'}){
      print "\n\nFVWresize()\n";
      print "config{'window_height'}=$config{'window_height'} \n";
      print "config{'window_width'}=$config{'window_width'} \n";
      my $cfw=$canvas->width;
      my $cfh=$canvas->height;
      my $bch=$barocanvas->height;
      my $cffh=$canvas_frame->height;
      print "canvas w = $cfw   \n";
      print "canvas h = $cfh   \n";
      print "baroconvh = $bch \n";
      print "canv_fra_h = $cffh \n";
    }

    if (($config{'window_width'} != $canvas_frame->width) || ($config{'window_height'} != $canvas_frame->height)) {

        #print "resize $config{'window_width'} ->".$canvas->width."\n";

        $config{'window_width'} = $canvas_frame->width;
        $config{'window_height'} = $canvas_frame->height;

        print "canvas_frame size is: $config{'window_width'} x $config{'window_height'} \n" if ($config{'DEBUG'});

        $baroheight = int (( ($config{'window_height'}) / $config{'fvw_baro_fraction'})+0.5)-2;
        $trackheight = int (( (1-(1/$config{'fvw_baro_fraction'}))*$config{'window_height'}  )+0.5)-2;

	# fix for rounding error!
        if ( ($baroheight+$trackheight) - $canvas_frame->height == -3) {
          $baroheight--;
        }

        #$trackheight = int (((2*$canvas_frame->height) / 3)+0.5)-2;

        # canvas will be two pixel larger than requested!
        # therefore we request smalle (-2)
        $barocanvas->configure(-width=>$config{'window_width'}-2, -height=>$baroheight, -scrollregion=>[0,0,$config{'window_width'}-2,$baroheight]);
        $canvas->configure(-width=>$config{'window_width'}-2, -height=>$trackheight, -scrollregion=>[0,0,$config{'window_width'}-2,$trackheight]);

        if ($config{'DEBUG'}) {
	  print "baroheight=$baroheight, trackheight=$trackheight   sum=".($baroheight+$trackheight)."\n";
        }

	#
        if ($config{'DEBUG'}){
	  $canvas->delete(@debugboxlist);
	  @debugboxlist=();
	  push(@debugboxlist, $canvas->createLine(0,$trackheight-1,$config{'window_width'}-3,$trackheight-1 ,-fill=>"red"));
	  push(@debugboxlist, $canvas->createLine($config{'window_width'}-3,0,$config{'window_width'}-3, $trackheight, -fill=>"red"));
	  push(@debugboxlist, $canvas->createLine(0,0,0,$trackheight,-fill=>"red"));
	  push(@debugboxlist, $canvas->createLine(0,0,$config{'window_width'}-3,0, -fill=>"red"));

	  $barocanvas->delete(@debugboxlist);
	  push(@debugboxlist, $barocanvas->createLine(0,$baroheight-1,$config{'window_width'}-3,$baroheight-1 ,-fill=>"green"));
	  push(@debugboxlist, $barocanvas->createLine($config{'window_width'}-3,0,$config{'window_width'}-3, $baroheight-1, -fill=>"green"));
	  push(@debugboxlist, $barocanvas->createLine(0,0,0,$baroheight-1,-fill=>"green"));
	  push(@debugboxlist, $barocanvas->createLine(0,0,$config{'window_width'}-3,0, -fill=>"green"));
	} else {
	  $barocanvas->delete(@debugboxlist);
	  $canvas->delete(@debugboxlist);
	}

        baroplot();

        if ($zoom == 1) {
            ($maxlon, $minlon, $maxlat, $minlat)=GPLIGCfunctions::Ausschnitt2($trackheight, $config{'window_width'}, $DECLAT[$nr], $DECLON[$nr], $config{'zoom_sidelength'});
        }

        if ($zoom == 2) {
            ($maxlon, $minlon, $maxlat, $minlat)=GPLIGCfunctions::Ausschnitt2($trackheight, $config{'window_width'}, $DECLAT[$nr], $DECLON[$nr], $kl2);
        }

	if ($filestat eq 'closed') {return;}

        $delta_lat = $maxlat-$minlat;
        if ($delta_lat == 0) {$delta_lat = 0.5;}
        $delta_lon = $maxlon-$minlon;
        if ($delta_lon == 0) {$delta_lon = 0.5;}

        $dyp=($delta_lat)/($trackheight-1);
        $dxp=($delta_lon)/($config{'window_width'}-3);

        if ($zoom ==0) {unzoomed();}

        mapplot($dxp, $dyp, $minlat, $minlon,"z");
        trackplot($dxp, $dyp, $minlat, $minlon);

        if ($task_state == 1) {taskdraw(0);taskdraw(1);}
        if ($wpcyl_state == 1) {wpcyldraw(0);wpcyldraw(1);}
        marksdraw();

        FVWausg();
  } else {
    print "nothing to resize\n" if ($config{'DEBUG'});
  }
}


# this function plots the markerlines  (crosshair and vertical line in baroview)
# and updates info-text
# wraps $nr if needed and changes shown area if marker outside
sub FVWausg {
    if ($filestat eq 'closed') {
        return 0;
    }
    if (!Exists($FlightView)) {return;}

#    print "FVWausg ---------\n" if ($config{'DEBUG'});

    #$SIG{'USR2'} = IGNORE;

    # falls ein Wert uebergeben wird uebergeben wird...
    # otherwise $nr is used as in global scope

#    print "FVWausg nr before $nr\n" if ($config{'DEBUG'});
    if (@_ >= 1) {
      $nr = shift;
    }

    # wrap, if outside of range
    if ($nr > $#LAT-1){$nr=0;}
    if ($nr < 0) {$nr=$#LAT-1;}

    print "FVWausg($nr) ----------- \n" if ($config{'DEBUG'});

    # OLD Code, maybe reactivated later
#    if ($^O ne "MSWin32") {
#        if (defined $oGLePID) {
#            if (kill 0, $oGLePID) {
#                if (defined $shm_id) {
#
#		    # if we havnt got sigusr2 from openGLIGCexplorer (FVWausg called from "inside-event")
#                    if ($sigusr2flag == 0){
#
#                        #write data in SysV IPC shared memory
#                        shmwrite($shm_id, pack("i", $nr) , 0, 4) || print "Error writing Shared Memory (SysV IPC)\n";
#
#                        #send USR1 to explorer (update signal)
#                        system("kill -USR1 $oGLePID");
#                    } else {
#
#			#print "Got SIGUSR2 from somewere... need to read shmem block here...\n";
#                        shmread($shm_id, $getfromshmem, 0, 4) || print "Error reading Shared Memory (SysV IPC)\n";
#                        $nr = unpack("i", $getfromshmem);
#
#                        #print "GPLIGC: $nr gelesen von shmem\n";
#                        $sigusr2flag=0;
#                    }
#                }
#            }
#        }
#    }

    # formatted strings for output
    my $speed =   sprintf("%.0f",$nonISPEED[$nr]*$config{'speed_unit_factor'});
    my $iaspeed = sprintf("%.0f",$IAS[$nr]*$config{'speed_unit_factor'});
    my $taspeed = sprintf("%.0f",$TAS[$nr]*$config{'speed_unit_factor'});

    # in GPS-mode IVARIO arrays are swapped with GPS arrays
    my $vario = sprintf("%.2f", $nonIVARIO[$nr]*$config{'vertical_speed_unit_factor'});
    my $intvario = sprintf("%.2f", $IVARIO[$nr]*$config{'vertical_speed_unit_factor'});
    my $compvario = sprintf("%.2f", $compVARIO[$nr]*$config{'vertical_speed_unit_factor'});

    my $htime=GPLIGCfunctions::time2human($TIME[$nr],'t');

    if ($config{'DEBUG'}){
        $htime .= " (index: $nr)";
    }

    my $dist_towp = 0;
    my $bear_towp= 0;
    my $wp_name="-";

    if ($MAXWP >= 1) {
        $dist_towp=sprintf("%.2f",GPLIGCfunctions::dist($DECLAT[$nr],$DECLON[$nr],$WPLAT[$AKTWP],$WPLON[$AKTWP])*$config{'distance_unit_factor'});
        $bear_towp= int (GPLIGCfunctions::kurs($DECLAT[$nr], $DECLON[$nr], $WPLAT[$AKTWP], $WPLON[$AKTWP]) + 0.5);

        if (length($bear_towp) == 2 ) { $bear_towp = "0".$bear_towp; }
        if (length($bear_towp) == 1 ) { $bear_towp = "00".$bear_towp; }
        $wp_name=$WPNAME[$AKTWP];
    } else {
        $dist_towp ="-"; $bear_towp="-";
    }

    my $heading = int(GPLIGCfunctions::kurs($DECLAT[$nr], $DECLON[$nr], $DECLAT[$nr+1], $DECLON[$nr+1]) + 0.5);
    if (length($heading) == 2) {$heading="0".$heading;}
    if (length($heading) == 1) {$heading="00".$heading;}

    my $format_lat=GPLIGCfunctions::coorconvert($DECLAT[$nr],'lat',$config{'coordinate_print_format'});
    my $format_lon=GPLIGCfunctions::coorconvert($DECLON[$nr],'lon',$config{'coordinate_print_format'});

    # in GPS-Mode BARO is swapped with GPSALT
    my $alt = sprintf("%.0f",$config{'altitude_unit_factor'}*$BARO[$nr]);
    my $gpsalt = sprintf("%.0f",$config{'altitude_unit_factor'}*$GPSALT[$nr]);
    $gpsalt = sprintf("%.0f(%.0f)",$config{'altitude_unit_factor'}*$GPSALT[$nr],$config{'altitude_unit_factor'}*$VXA[$nr]) if ($VXAEXISTS eq "yes" && $config{'draw_accuracy'});
    my $press = sprintf("%.1f",GPLIGCfunctions::pressure($BARO[$nr],$gpi{'qnh'}));
    my $opress = sprintf("%.1f",GPLIGCfunctions::pressure($BARO[$nr],$gpi{'qnh'})*0.20942);

    my $altmode_txt = "Baro";
    if ($altmode eq "gps") {$altmode_txt="GPS-Altitude";}

    my $pressline="";

    if ($gpi{'qnh'} != 0) {
        $pressline=sprintf("  QNH (hPa): %.0f  Pressure altitude: %.0f $config{'altitude_unit_name'}",$gpi{'qnh'},$config{'altitude_unit_factor'}*(GPLIGCfunctions::altitude(GPLIGCfunctions::pressure($BARO[$nr],$gpi{'qnh'}),1013.25)));
    }

    $fvwtext = "Coordinates: $format_lat   $format_lon  Time (UTC): $htime  No.: $nr  $altmode_txt: $alt $config{'altitude_unit_name'}   GPS: $gpsalt $config{'altitude_unit_name'}\nPressure $press hPa  pO2 $opress hPa$pressline\n";
    $fvwtext.= "Groundspeed: $speed $config{'speed_unit_name'}   IAirspeed: $iaspeed $config{'speed_unit_name'}  TAirspeed: $taspeed $config{'speed_unit_name'}   Heading: $heading";

    if ($SIUEXISTS eq "yes") {
        $fvwtext .= "  Sats used: $SIU[$nr]";
    }
    if ($FXAEXISTS eq "yes") {
        $fvwtext.="  Acc.Hor: ".(int($FXA[$nr]*$config{'altitude_unit_factor'}))." [$config{'altitude_unit_name'}]";
    }
    if ($VXAEXISTS eq "yes") {
        $fvwtext.="  Acc.Vert: ".(int($VXA[$nr]*$config{'altitude_unit_factor'}))." [$config{'altitude_unit_name'}]";
    }

    $fvwtext.="\n";
    $fvwtext.="Vario: $vario $config{'vertical_speed_unit_name'}   int.Vario: $intvario $config{'vertical_speed_unit_name'}  TEcompVario: $compvario $config{'vertical_speed_unit_name'}\n";
    $fvwtext.="To $wp_name: $dist_towp $config{'distance_unit_name'} , $bear_towp";

    if ($config{'maps'}) {
        $fvwtext.=", Map zoomlevel/Tiles: $zl / $tiles";
    }

    if ($altmode eq "gps") { $fvwtext.="\nGPS-Altitude modus!";}

    $canvas->delete($horizontal, $vertical);
    $barocanvas->delete($barovertical);

    if ($FXAEXISTS eq "yes" ) {
      if (@acc_lines) {$canvas->delete(@acc_lines);}
      @acc_lines = ();
    }

    my $x= int(($DECLON[$nr]-$minlon)/$dxp);
    my $y= int($trackheight-(($DECLAT[$nr]-$minlat)/$dyp));
    my $xbaro = int($nr/$dxbaro);

    if ($cut_track == 1 && !($stat_start == 0 && $stat_end == $#BARO) && $stat_end > $stat_start) {
        $xbaro = int(($nr-$stat_start)/$dxbaro);
    }

    #drawing lines (horizontal/vertical)
    $horizontal=$canvas->createLine(0,$y,$config{'window_width'},$y,-fill=>"navy blue");
    $vertical=$canvas->createLine($x,0,$x,$trackheight,-fill=>"navy blue");
    $barovertical = $barocanvas->createLine($xbaro,0,$xbaro,$baroheight,-fill=>"navy blue");

    #draw accuracy circle if wanted
    if ($FXAEXISTS eq "yes" && $config{'draw_accuracy'}) {

        my ($cylref_lat, $cylref_lon) = GPLIGCfunctions::zylinder2( $DECLAT[$nr], $DECLON[$nr], ($FXA[$nr]/1000), 0, 359.99999);

        for  (my $xzaehl=0; $xzaehl<=$#$cylref_lat-1; $xzaehl++) {

	    my $x1= int( ( ($DECLON[$nr]+${$cylref_lon}[$xzaehl]) -$minlon) /$dxp);
            my $x2= int( ( ($DECLON[$nr]+${$cylref_lon}[$xzaehl+1]) -$minlon) /$dxp);
            my $y1= int($trackheight-( ( ($DECLAT[$nr]+${$cylref_lat}[$xzaehl])   -$minlat )/$dyp));
            my $y2= int($trackheight-( ( ($DECLAT[$nr]+${$cylref_lat}[$xzaehl+1]) -$minlat )/$dyp));
            push(@acc_lines, $canvas->createLine( $x1, $y1, $x2, $y2,-width=>1,-fill=>"navy blue"));
        }

    }

#    if ($config{'DEBUG'}) {
#      if ($x > $config{'window_width'} || $x < 0) { print "X is outside range ($x)!\n";}
#      if ($y > $trackheight || $y <0) { print "Y is outside range ($y)!\n";}
#    }

    if ($x > $config{'window_width'} || $x < 0) { if ($zoom==2) {zoom2(1);} else {$zoom=0; zoom(); } return; }
    if ($y > $trackheight || $y <0) {if ($zoom==2) {zoom2(1);} else {$zoom=0; zoom(); } return; }


#$SIG{'USR2'} = sub {$sigusr2flag=1; print "sigusr2 got... (FVW)\n"; FVWausg();$FlightView->update;};

}

#############################################################################

sub WpPlot {

    #WPPlotWindow already exists?
    if (Exists($WPPlotWindow)) {$WPPlotWindow->destroy;}

    if ($filestat eq 'closed') {$FlightView->bell; return;}
    $WPPlotWindow=$FlightView->Toplevel();
    $WPPlotWindow->configure(-title=>"Task Editor");

    setexit($WPPlotWindow);

    my $pltb2d=$WPPlotWindow->Button(-text=>"plot 2D",-command=>sub{

            if ($MAXWP > 0) {
                zylinder($WPLAT[$AKTWP],$WPLON[$AKTWP],$config{'zylinder_radius'}*($WPRADFAC[$AKTWP]+0.000000000001),"1000");
                $com1="plot";
                $com2="\'${pfadftmp}zyl.dat\' title \"Zylinder"."($config{'zylinder_radius'}*$WPRADFAC[$AKTWP]) $WPNAME[$AKTWP] "."\" with lines, \'${pfadftmp}3d.dat\' title \"Flightpath\" ".$config{'gnuplot_draw_style'};
                gnuplot("2d");
                $xmin = $xmax = $ymin = $ymax = $zmax = ''; $zmin = 0;
            }
          });

    $pltb2d->pack(-fill=>"x");

    my $pltb3d=$WPPlotWindow->Button(-text=>"plot 3D",-command=>sub{

            if ($MAXWP > 0 ) {
                zylinder($WPLAT[$AKTWP],$WPLON[$AKTWP],$config{'zylinder_radius'}*($WPRADFAC[$AKTWP]+0.000000000001),"1000");
                $com1="splot";
                $com2="\'${pfadftmp}zyl.dat\' title \"Zylinder"."($config{'zylinder_radius'}*$WPRADFAC[$AKTWP]) $WPNAME[$AKTWP]"."\" with dots, \'${pfadftmp}3d.dat\' using (\$1):(\$2):(\$3*0) title \"Groundtrack\" ".$config{'gnuplot_draw_style'}.", \'${pfadftmp}3d.dat\' title \"Flightpath\" ".$config{'gnuplot_draw_style'};
                gnuplot("3d");
                $xmin = $xmax = $ymin = $ymax = $zmax = ''; $zmin = 0;
            }
          });

    $pltb3d->pack(-fill=>"x");

    $myWPN=$WPNAME[$AKTWP];
    $myWPRF=$WPRADFAC[$AKTWP]; # WP Radius Factor local var
    $wppwlat=GPLIGCfunctions::coorconvert($WPLAT[$AKTWP],'lat',$config{'coordinate_print_format'});
    $wppwlon=GPLIGCfunctions::coorconvert($WPLON[$AKTWP],'lon',$config{'coordinate_print_format'});

    my $wpframe=$WPPlotWindow->Frame();
    my $LB=$wpframe->Button(-text=>'<<',-command=>sub{
            if ($MAXWP > 0) {
		$WPRADFAC[$AKTWP]=$myWPRF;
                $AKTWP--;
                if ($AKTWP==0) {$AKTWP++;}
                $myWPN=$WPNAME[$AKTWP];
                $myWPRF=$WPRADFAC[$AKTWP];
                $wppwlat=GPLIGCfunctions::coorconvert($WPLAT[$AKTWP],'lat',$config{'coordinate_print_format'});
                $wppwlon=GPLIGCfunctions::coorconvert($WPLON[$AKTWP],'lon',$config{'coordinate_print_format'});
                FVWausg();
                TaskUpdate();
            }
          });

    my $WPANZ=$wpframe->Label(-textvariable=>\$AKTWP);
    my $RB=$wpframe->Button(-text=>'>>',-command=>sub{
            if ($MAXWP > 0) {
		$WPRADFAC[$AKTWP]=$myWPRF;
                $AKTWP++;
                if ($AKTWP>$MAXWP) {$AKTWP--;}
                $myWPN=$WPNAME[$AKTWP];
                $myWPRF=$WPRADFAC[$AKTWP];
                $wppwlat=GPLIGCfunctions::coorconvert($WPLAT[$AKTWP],'lat',$config{'coordinate_print_format'});
                $wppwlon=GPLIGCfunctions::coorconvert($WPLON[$AKTWP],'lon',$config{'coordinate_print_format'});
                FVWausg();
                TaskUpdate();
            }
          });

    $LB->pack(-expand=>"yes",-side=>"left",-fill=>"x");
    $WPANZ->pack(-side=>"left",-fill=>"x");

    $RB->pack(-expand=>"yes",-side=>"left",-fill=>"x");

    $wpframe->pack(-expand=>"yes",-fill=>"x");

    my $wpndf=$WPPlotWindow->Label(-textvariable=>\$myWPN);

    my $rfframe=$WPPlotWindow->Frame();

    my $WPRFl=$rfframe->Label(-text=>"Radius factor");
    my $WPRF=$rfframe->Entry(-textvariable=>\$myWPRF);
    $wpndf->pack(-fill=>"x");

    $WPRFl->pack(-expand=>"yes",-side=>"left",-fill=>"x");
    $WPRF->pack(-expand=>"yes",-side=>"left",-fill=>"x");
    $rfframe->pack(-fill=>"x");

    my $coor_lat=$WPPlotWindow->Label(-textvariable=>\$wppwlat);
    my $coor_lon=$WPPlotWindow->Label(-textvariable=>\$wppwlon);
    $coor_lat->pack(-fill=>"x");
    $coor_lon->pack(-fill=>"x");

    my $savebut=$WPPlotWindow->Button(-text=>"Save Task (.tsk)",-command=>sub{
      save_tsk();
    })->pack(-fill=>"x");

    my $delbut=$WPPlotWindow->Button(-text=>"Delete Waypoint",-command=>sub{
            if ($MAXWP > 0) {
                $MAXWP--;

                #removes 1 element at $AKTWP!
                splice @WPNAME, $AKTWP, 1;
                splice @WPLAT, $AKTWP, 1;
                splice @WPLON, $AKTWP, 1;

               #if task changed since optimization loose ability to olc-file output
                undef $olcFirstWPindex;
                $optimized_for = "none";

                if ($AKTWP > $MAXWP) {
                    $AKTWP--;
                }

                if ($AKTWP == 0) {
                    $AKTWP++;
                    $myWPN="All Waypoints deleted";
                    $wppwlat='-';
                    $wppwlon='-';
                    $freeWP = 1;
                } else {
                    $myWPN=$WPNAME[$AKTWP];
                    $wppwlat=GPLIGCfunctions::coorconvert($WPLAT[$AKTWP],'lat',$config{'coordinate_print_format'});
                    $wppwlon=GPLIGCfunctions::coorconvert($WPLON[$AKTWP],'lon',$config{'coordinate_print_format'});
                }

                print "No of WPs (maxwp): $MAXWP // Actual WP (aktwp): $AKTWP // Size of array (\$#WPLAT): $#WPLAT \n" if ($config{'DEBUG'});

                FVWausg();
                FSupdate();
                TaskUpdate();
                baroplot();#_task();
            }
          })->pack(-fill=>"x");

    my $last_first=$WPPlotWindow->Button(-text=>"Set last point same as first",-command=>sub{
	    if ($MAXWP == 0) {return;}
            push @WPNAME, $WPNAME[1];
            push @WPLAT, $WPLAT[1];
            push @WPLON, $WPLON[1];
            $MAXWP++;

            #if task changed since optimization loose ability to olc-file output
            undef $olcFirstWPindex;
            $optimized_for = "none";

            FSupdate();
            TaskUpdate();

          });

    $last_first->pack(-fill=>"x");

    #-----------------------------------

    # THE REAL OPTIMIZATION
    # using the cpp program optimizer....

    my $optiV3=$WPPlotWindow->Button(-text=>"Optimize for OLC-Classic 10/2007", -command=>sub{ optimizer("olc-class"); })->pack(-fill=>"x");
    my $optiV3a=$WPPlotWindow->Button(-text=>"Optimize for OLC-Fai 10/2007", -command=>sub{ optimizer("olc-fai"); })->pack(-fill=>"x");
    my $optiV4=$WPPlotWindow->Button(-text=>"Optimize for DMSt 2006", -command=>sub{ optimizer("dmst"); })->pack(-fill=>"x");
    my $optiV5=$WPPlotWindow->Button(-text=>"Optimize for OLC-Phg 3/2007", -command=>sub{ optimizer("olc-phg"); })->pack(-fill=>"x");

    my $exb=$WPPlotWindow->Button(-text=>"Exit",-command=>sub{$WPPlotWindow->destroy();});
    $exb->pack(-fill=>"x");

}

########################################

# this function updates the variables for output in WpPlotWindow
sub WPPlotUpdate {

    #print "$myWPN $wppwlat  $wppwlon\n";

    $myWPN=$WPNAME[$AKTWP];
    $myWPRF=$WPRADFAC[$AKTWP];
    $wppwlat=GPLIGCfunctions::coorconvert($WPLAT[$AKTWP],'lat',$config{'coordinate_print_format'});
    $wppwlon=GPLIGCfunctions::coorconvert($WPLON[$AKTWP],'lon',$config{'coordinate_print_format'});

    #print "$myWPN $wppwlat  $wppwlon\n";

}

#####################################################################################

sub Ausschnitt {

    use Math::Trig;

    my ($lat, $lon) = @_;
    my $halbkilometer=0.004496608;

    $xmin=$lon-($config{'zoom_sidelength'}*$halbkilometer*(1/cos(deg2rad($lat))));
    $xmax=$lon+($config{'zoom_sidelength'}*$halbkilometer*(1/cos(deg2rad($lat))));
    $ymin=$lat-($config{'zoom_sidelength'}*$halbkilometer);
    $ymax=$lat+($config{'zoom_sidelength'}*$halbkilometer);
}

################################################################################

sub TaskUpdate {
    if (Exists($FlightView)) {
        if ($task_state == 1) { taskdraw(0); taskdraw(1);}
        if ($wpcyl_state == 1) { wpcyldraw(0); wpcyldraw(1); }
    }
}

###################### elevation calibration

sub elevationcalibration {

    my $caltype= shift;
    my $elevation = shift;
    my $nx = shift;
    my $initflag = shift; # =1 for initial call (file opening) to avoid trackplot


    if (!defined $elevation) { Errorbox("cancelled");
        return;
    }


    if ($caltype eq "constant"){
        my $delta = $BARO[$nx] - ($elevation/$config{'altitude_unit_factor'});
        $offset -= int($delta+.5);

        for (my $z=0; $z <= $#BARO; $z++) {
            $BARO[$z] -= $delta;
        }

        $minbaro -= $delta;
        $maxbaro -= $delta;

        $gpi{'altitude_calibration_type'}=$caltype;
        $gpi{'altitude_calibration_point'}=$nx;
        $gpi{'altitude_calibration_point_alt'}=$elevation;
    }

    if ($caltype eq "baro") {

        my $p0 = GPLIGCfunctions::referencepressure($BARO[$nx], GPLIGCfunctions::pressure($elevation,$gpi{'qnh'}) );

        for (my $z=0; $z <= $#BARO; $z++) {
            $BARO[$z] = GPLIGCfunctions::altitude(GPLIGCfunctions::pressure($BARO[$z],$p0),$gpi{'qnh'});
        }

        $offset = -int(($minbaro - GPLIGCfunctions::altitude(GPLIGCfunctions::pressure($minbaro,$p0),$gpi{'qnh'}))+.5);
        $minbaro = GPLIGCfunctions::altitude(GPLIGCfunctions::pressure($minbaro,$p0),$gpi{'qnh'});
        $maxbaro = GPLIGCfunctions::altitude(GPLIGCfunctions::pressure($maxbaro,$p0),$gpi{'qnh'});

        $gpi{'altitude_calibration_type'}=$caltype;
        $gpi{'altitude_calibration_point'}=$nx;
        $gpi{'altitude_calibration_point_alt'}=$elevation;
    }

    if (Exists($FlightView)){
        updateFVW() if (!$initflag);
    }

    tmpfileout();
    FSupdate();
}

#################################

sub calibrationinput {

    if ($gpi{'altitude_calibration_type'} ne "none") { Errorbox("applying another calibration may result in weird effects!");}

    my $input=$FlightView->Toplevel();
    my $elevation=0;
    my $caltype ="baro";
    $input->configure(-title=>"Elevation input");

    my $eleframe=$input->Frame()->pack();
    my $radfr=$input->Frame()->pack;
    my $bfr=$input->Frame()->pack();

    $eleframe->Label(-text=> "Elevation at selected point ($config{'altitude_unit_name'})")->pack(-side=>"left");
    $eleframe->Entry(-textvariable=>\$elevation)->pack(-side=>"right");

    $radfr->Radiobutton(-text=>"constant shift",-variable=>\$caltype,-value=>"constant")->pack();
    $radfr->Radiobutton(-text=>"barometric",-variable=>\$caltype,-value=>"baro")->pack();

    $bfr->Button(-text=>"OK",-command=>sub{
            $input->destroy();
            elevationcalibration($caltype, $elevation, $nr, 0);
          })->pack(-side=>"left");
    $bfr->Button(-text=>"cancel",-command=>sub{
            $input->destroy();
            undef $elevation;
            return; # back to nix
          })->pack(-side=>"right");

    $input -> waitWindow;
}

sub qnhcalibration {

    if ($gpi{'altitude_calibration_type'} ne "none") { Errorbox("applying another calibration may result in weird effects!");}
    #qnhcalibrationinput();
    my $newqnh=shift;
    my $newrefp=shift;
    my $initflag = shift;

    if ($newqnh < 800 || $newqnh >1200) { Errorbox("cancelled");
        return;
    }

    $gpi{'qnh'}=$newqnh;

    #my $pressdiff = 1013.25 - $gpi{'qnh'};

    if ($newqnh != 0 && $newrefp != 0) {

        for (my $z=0; $z <= $#BARO; $z++) {
            $BARO[$z] = GPLIGCfunctions::altitude(GPLIGCfunctions::pressure($BARO[$z],$newrefp),$newqnh);
        }

        $offset = -int(($minbaro - GPLIGCfunctions::altitude(GPLIGCfunctions::pressure($minbaro,$newrefp),$newqnh))+.5);
        $minbaro = GPLIGCfunctions::altitude(GPLIGCfunctions::pressure($minbaro,$newrefp),$newqnh);
        $maxbaro = GPLIGCfunctions::altitude(GPLIGCfunctions::pressure($maxbaro,$newrefp),$newqnh);

        $gpi{'altitude_calibration_type'}="refpress";
        $gpi{'altitude_calibration_ref_press'}=$newrefp;
    }

    tmpfileout();
    if (Exists($FlightView)){
        updateFVW() if (!$initflag);
    }
    FSupdate();

    ##########################################

}

sub qnhcalibrationinput {
    my $input=$FlightView->Toplevel();
    my $newqnh=$gpi{'qnh'};
    my $newrefp=0;

    $input->configure(-title=>"Pressure calibration");

    my $eleframe=$input->Frame()->pack();
    my $elevf2=$input->Frame()->pack();
    my $bfr=$input->Frame()->pack();

    $eleframe->Label(-text=> "QNH (hPa)")->pack(-side=>"left");
    $eleframe->Entry(-textvariable=>\$newqnh)->pack(-side=>"right");
    $elevf2->Label(-text=> "Reference pressure of logged altitude (hPa)")->pack(-side=>"left");
    $elevf2->Entry(-textvariable=>\$newrefp)->pack(-side=>"right");

    $bfr->Button(-text=>"OK",-command=>sub{
            $input->destroy();
            qnhcalibration($newqnh,$newrefp,0);
            return;
          })->pack(-side=>"left");
    $bfr->Button(-text=>"cancel",-command=>sub{
            $input->destroy();
            return;
          })->pack(-side=>"right");

    $input -> waitWindow;
}

########################################

sub statistik {

    my ($start, $end) = @_;

    if ($start >= $end) {Errorbox("Endpoint (F6) is before Startpoint (F5) - swapping"); my $tmp = $start; $start = $end; $end = $tmp;}
    if ($start == $end) {Errorbox("Start (F5) and end (F6) are the same!"); return;}

    my $delta_h = $BARO[$end]-$BARO[$start];
    my $delta_h_unit = $delta_h;

    my $delta_t = $DECTIME[$end]-$DECTIME[$start];
    my $delta_th = GPLIGCfunctions::dec2time($delta_t);
    my $climb = ( ($delta_h/1000) / $delta_t) / 3.6;
    my $time = GPLIGCfunctions::time2human($TIME[$start],'t')."-".GPLIGCfunctions::time2human($TIME[$end],'t');

    $delta_h_unit *= $config{'altitude_unit_factor'};
    $climb *= $config{'vertical_speed_unit_factor'};

    my($ofilename, $odirectories, $osuffix) = File::Basename::fileparse($file);
    my $message = "File: $ofilename\n";

    $message .= sprintf("Time: $time\nTime: $delta_th\nAltitude difference: $delta_h_unit $config{'altitude_unit_name'} \nClimb rate: %.1f $config{'vertical_speed_unit_name'}", $climb );

    my $distance = GPLIGCfunctions::dist($DECLAT[$start], $DECLON[$start], $DECLAT[$end], $DECLON[$end]);
    my $speed = $distance / $delta_t;
    my $glide;
    if ($delta_h != 0) {$glide = ($distance*1000) / -$delta_h;} else {$glide = 9999;}

    $distance *= $config{'distance_unit_factor'};
    $speed *= $config{'speed_unit_factor'};

    $message .= sprintf("\nDistance: %.3f $config{'distance_unit_name'} \nSpeed: %.0f $config{'speed_unit_name'}", $distance, $speed);
    $message .= sprintf("\nGlide ratio: %.1f", $glide);

    my $head = GPLIGCfunctions::kurs($DECLAT[$start], $DECLON[$start], $DECLAT[$end], $DECLON[$end]);

    $message .= sprintf("\nCourse: %.0f",$head);

    my $cum_dist =0;
    my $cum_climb = 0;
    my $cum_sink = 0;

    my %wind;
    my %windcount;
    my @minval=(0);
    my @maxval=(0);
    my @mink=(0);
    my @maxk=(0);

    for (my $z=$start; $z<$end; $z++) {
        $cum_dist += GPLIGCfunctions::dist($DECLAT[$z], $DECLON[$z], $DECLAT[$z+1], $DECLON[$z+1]);
        $cum_climb += $BARO[$z+1]-$BARO[$z]  if ($BARO[$z+1]-$BARO[$z] > 0);
        $cum_sink += $BARO[$z+1]-$BARO[$z]  if ($BARO[$z+1]-$BARO[$z] < 0);

	my $diff;
	if ($TASEXISTS eq "yes") {
	    $diff = $TAS[$z] - (($nonISPEED[$z]+$nonISPEED[$z+1])/2)  ;
	} else {
	    $diff = $IAS[$z] - (($nonISPEED[$z]+$nonISPEED[$z+1])/2)  ;
	};


        my $kurs = GPLIGCfunctions::kurs($DECLAT[$z-1], $DECLON[$z-1], $DECLAT[$z+1], $DECLON[$z+1]);
        my $roundk = int($kurs+0.5);

        $wind{$roundk} += $diff;
        $windcount{$roundk} ++;
    }

    foreach $key (keys %wind) {
        if ($windcount{$key} > 1) {$wind{$key} = $wind{$key}/$windcount{$key}}
    }

    open (WIND,">${pfadftmp}wind.dat");
    for (my $z=0;$z<=361;$z++) {
        if (exists($wind{$z})) {print WIND "$z  $wind{$z}\n";}
    }
    close(WIND);

    if ($config{'wind_analysis'}) {
        $com1="f(x)=a*sin((x/57.29578)+(-b/57.29578))+c \na=150\nb=45\nc=-100\n g(x) = c \n fit f(x) \'${pfadftmp}wind.dat\' via a,b,c \n set ylabel \"airspeed - groundspeed [$config{'speed_unit_name'}]\" \n set xlabel \"direction \" \n set xrange [0:360]\n";
        $com1.= "set label 1 \"offset = %.1f km/h\",c at graph 0.02, graph 0.95 \n";
        $com1.= "set label 2 \"amplitude = %.1f km/h\",a at graph 0.02, graph 0.90\n";
        $com1.= "set label 3 \"direction = %.1f\",b+90 at graph 0.02, graph 0.85\n plot ";
        $com2="\'${pfadftmp}wind.dat\' using (\$1):(\$2*$config{'speed_unit_factor'}) title \"windplot from last f5f6f7\" ".$config{'gnuplot_draw_style'}.",f(x), g(x) \n print \"Speed offset: \", c,\" km/h\"\n print \"Maxima \", b+90, \" grad\"\n print \"Speed \", a, \" km/h\"";
        gnuplot("2d");
    }

    $message .= sprintf("\nCumulated climb: %.0f $config{'altitude_unit_name'}", $cum_climb * $config{'altitude_unit_factor'});
    $message .= sprintf("\nCumulated sink: %.0f $config{'altitude_unit_name'}", $cum_sink * $config{'altitude_unit_factor'});
    $message .= sprintf("\nCumulated distance: %.3f $config{'distance_unit_name'}", $cum_dist * $config{'distance_unit_factor'});

    my $speed_cum_dist = $cum_dist / $delta_t;

    my $glideratio_cum = 77777; # just big for div/0
    if ($delta_h != 0) {$glideratio_cum =  ($cum_dist * 1000 ) / -$delta_h}
    $message .= sprintf("\nSpeed : %.0f $config{'speed_unit_name'}", $speed_cum_dist * $config{'speed_unit_factor'});
    $message .= sprintf("\nGlide ratio cumulated distance: %.1f", $glideratio_cum);

    MessageFenster("Statistic", $message);
}

########################################

sub thermik_statistik {

    @thermal_start_time_index = ();
    my @thermal_end_time_index = ();
    my @circled=();
    my @circled_abs =();

    # default values for thermal search
    my $circles = 2;	#how many circles needed
    my $steps = 3;
    my $heading_delta = 15;

    my $in_thermal = 'no';
    my $in_counter = 0;
    my $out_counter = 0;
    my $circle_sum_abs = 0;
    my $circle_sum = 0;
    my $tmp_start_index = 0;

    for (my $z= $task_start_time_index; $z < $task_finish_time_index; $z++) {

        my $k_alt = GPLIGCfunctions::kurs($DECLAT[$z-1], $DECLON[$z-1], $DECLAT[$z], $DECLON[$z]);
        my $k_new = GPLIGCfunctions::kurs($DECLAT[$z], $DECLON[$z], $DECLAT[$z+1], $DECLON[$z+1]);
        my $k_delta = $k_alt-$k_new;

        if (abs($k_delta) > 180) {

            #print "***";
            if ($k_delta < 0) {$k_delta += 360;} else {$k_delta -= 360;}
        }

        $circle_sum_abs += abs($k_delta);
        $circle_sum += $k_delta;

        if ($in_thermal eq 'no') {

            #print "out\n";
            if (abs($k_delta) < $heading_delta) { $in_counter=0; $circle_sum=0; $circle_sum_abs=0;}
            if (abs($k_delta) > $heading_delta) { $in_counter++;}
            if ($in_counter== $steps) {$in_thermal='yes'; $tmp_start_index = $z - $steps;}
        }

        if ($in_thermal eq 'yes') {

            #print "in\n";
            if (abs($k_delta) > $heading_delta) { $out_counter=0; }
            if (abs($k_delta) < $heading_delta) { $out_counter++;}

            if ($out_counter == $steps) {
                $in_thermal = 'no';
                $in_counter=0;

                #print "summe ==== $circle_sum\n";

                if (abs($circle_sum_abs) > ($circles*360)) {
                    push (@thermal_start_time_index, $tmp_start_index);
                    push (@thermal_end_time_index, $z - $steps);
                    push (@circled_abs, $circle_sum_abs);
                    push (@circled, $circle_sum);

                    #print "thermal !!!!!!!!!\n";
                }

                $circle_sum=0; $circle_sum_abs=0;
            }
        }

    }

    if (Exists($thermal_statistik)) {$thermal_statistik->destroy;}

    $thermal_statistik=$FlightView->Toplevel();
    $thermal_statistik->configure(-title=>"Thermals");
    setexit($thermal_statistik);

    $therm_list_box = $thermal_statistik->ScrlListbox(-label=>"Thermals", -selectmode=>"single",-height=>"0",-width=>"0")->pack(-side=>"left");
    $therm_list_box->bind("<Double-Button-1>" => \&therm_click);

    my $total_climb_alt = 0;
    my $total_climb_t = 0;
    my $thermal_stat_text = '';

    my($ofilename, $odirectories, $osuffix) = File::Basename::fileparse($file);
    $thermal_stat_text .= "File: $ofilename\n";

    my $thermal_r = 0;
    my $thermal_l = 0;
    my $thermal_lr = 0;
    my $best = 0;
    my $worst = 10;

    for (my $z=0; $z<= $#thermal_start_time_index; $z++) {

        my $list_zeile='';
        $list_zeile .=  GPLIGCfunctions::time2human($TIME[$thermal_start_time_index[$z]],'t')."-".GPLIGCfunctions::time2human($TIME[$thermal_end_time_index[$z]],'t')." ";

        $list_zeile .= GPLIGCfunctions::dec2time($DECTIME[$thermal_end_time_index[$z]] - $DECTIME[$thermal_start_time_index[$z]])." ";

        my $circles = $circled[$z] / 360;
        my $circles_abs = $circled_abs[$z] / 360;

        $list_zeile .= sprintf ("  %3.0f", $circles_abs);

        if ( abs(abs($circles)-$circles_abs) < 1  ) {
            if ($circles < 0) {$list_zeile .= "R  "; $thermal_r++;}
            if ($circles > 0) {$list_zeile .= "L  "; $thermal_l++;}
        } else {

            $list_zeile .= "R/L";
            $thermal_lr++;

            #print "$circles   $circles_abs\n";

        }

        my $delta_h = $BARO[$thermal_end_time_index[$z]] - $BARO[$thermal_start_time_index[$z]];

        $total_climb_alt += $delta_h;
        my $delta_t = $DECTIME[$thermal_end_time_index[$z]] - $DECTIME[$thermal_start_time_index[$z]];

        $total_climb_t += $delta_t;
        my $climb_rate = ( ($delta_h/1000) / $delta_t) / 3.6;

        if ($climb_rate < $worst) {$worst = $climb_rate;}
        if ($climb_rate > $best) {$best = $climb_rate;}

        $list_zeile .= sprintf ("  %4.0f $config{'altitude_unit_name'}   %2.1f $config{'vertical_speed_unit_name'}", $delta_h*$config{'altitude_unit_factor'}, $climb_rate*$config{'vertical_speed_unit_factor'});

        #print $list_zeile."\n";

        $therm_list_box->insert("end", $list_zeile);
    }

    #statistics window:
    my $statw_tot_alt = sprintf("%.0f",$total_climb_alt*$config{'altitude_unit_factor'});
    $thermal_stat_text .= "Total height climbed in circles:  $statw_tot_alt $config{'altitude_unit_name'}\n";
    my $total_climb_rate = (($total_climb_alt/1000) / $total_climb_t) / 3.6;
    $total_climb_rate *= $config{'vertical_speed_unit_factor'};
    $thermal_stat_text .= sprintf ("Average climb rate:  %.2f $config{'vertical_speed_unit_name'}\n", $total_climb_rate);

    my $circling = $total_climb_t / ($DECTIME[$task_finish_time_index] - $DECTIME[$task_start_time_index]) *100;
    $thermal_stat_text .= sprintf ("Circling: %.1f", $circling)."\%\n";

    $thermal_stat_text .= sprintf("Best: %.2f $config{'vertical_speed_unit_name'}\nWorst: %.2f $config{'vertical_speed_unit_name'}\n", $best*$config{'vertical_speed_unit_factor'}, $worst*$config{'vertical_speed_unit_factor'});

    my $left_perc = $thermal_l / ($thermal_l+$thermal_r+$thermal_lr) * 100;
    my $right_perc = $thermal_r / ($thermal_l+$thermal_r+$thermal_lr) * 100;
    my $leftright_perc = $thermal_lr / ($thermal_l+$thermal_r+$thermal_lr) * 100;
    $thermal_stat_text .= "# of thermals:  ".($thermal_l+$thermal_r+$thermal_lr)."\n";
    $thermal_stat_text .= sprintf ("Thermals left:  %.0f", $left_perc)."\% ($thermal_l)\n";
    $thermal_stat_text .= sprintf ("Thermals right:  %.0f", $right_perc)."\% ($thermal_r)\n";
    $thermal_stat_text .= sprintf ("Thermals left and right:  %.0f", $leftright_perc)."\% ($thermal_lr)\n";

    if ( $thermal_l+$thermal_r+$thermal_lr > 0) {
        MessageFenster("Thermal statistics", $thermal_stat_text);
    } else {
        if (Exists($thermal_statistik)) {$thermal_statistik->destroy;}
        Errorbox ("No thermals found");
    }

    sub therm_click {

        #my $sel=$therm_list_box->curselection();
        #$nr = $thermal_start_time_index[@$sel[0]];
        #FVWausg($thermal_start_time_index[@$sel[0]]);
        my @sel=$therm_list_box->curselection();
        if (defined $sel[0]) {
            $nr = $thermal_start_time_index[$sel[0]];
            FVWausg($thermal_start_time_index[$sel[0]]);
        }
    }

}

########################################

sub glide_statistik {

    @glide_start_time_index = ();
    my @glide_end_time_index = ();
    my @glide_head = ();
    my @glide_speed = ();
    my @glide_dist=();

    # default values for glide search

    my $steps = 5;
    my $heading_delta = 10;
    my $min_km = 3;		#min km

    my $in_thermal = 'yes';
    my $in_counter = 0;
    my $out_counter = 0;
    my $tmp_start_index = 0;

    for (my $z= $task_start_time_index; $z < $task_finish_time_index; $z++) {

        #print "$z \n";

        my $k_alt = GPLIGCfunctions::kurs($DECLAT[$z-1], $DECLON[$z-1], $DECLAT[$z], $DECLON[$z]);
        my $k_new = GPLIGCfunctions::kurs($DECLAT[$z], $DECLON[$z], $DECLAT[$z+1], $DECLON[$z+1]);
        my $k_delta = $k_alt-$k_new;

        if (abs($k_delta) > 180) {

            #print "***";
            if ($k_delta < 0) {$k_delta += 360;} else {$k_delta -= 360;}
        }

        if ($in_thermal eq 'no') {

            #print "out\n";
            if (abs($k_delta) < $heading_delta) { $in_counter=0; }#$circle_sum=0; $circle_sum_abs=0;
            if (abs($k_delta) > $heading_delta) { $in_counter++;}
            if ($in_counter== $steps) {
                $in_thermal='yes';
                if (GPLIGCfunctions::dist($DECLAT[$z-$steps], $DECLON[$z-$steps], $DECLAT[$tmp_start_index], $DECLON[$tmp_start_index]) > $min_km) {
                    push (@glide_start_time_index, $tmp_start_index);
                    push (@glide_end_time_index, $z - $steps);
                    push (@glide_head, GPLIGCfunctions::kurs($DECLAT[$tmp_start_index], $DECLON[$tmp_start_index], $DECLAT[$z-$steps], $DECLON[$z-$steps]));
                    my $speed = GPLIGCfunctions::dist($DECLAT[$z-$steps], $DECLON[$z-$steps], $DECLAT[$tmp_start_index], $DECLON[$tmp_start_index]) /
                      ($DECTIME[$z-$steps]-$DECTIME[$tmp_start_index]);
                    push (@glide_speed, $speed);

                    push (@glide_dist, GPLIGCfunctions::dist($DECLAT[$z-$steps], $DECLON[$z-$steps], $DECLAT[$tmp_start_index], $DECLON[$tmp_start_index]));
                }
            }
        }

        if ($in_thermal eq 'yes') {

            #print "in\n";
            if (abs($k_delta) > $heading_delta) { $out_counter=0; }
            if (abs($k_delta) < $heading_delta) { $out_counter++;}
            if ($out_counter == $steps) {
                $in_thermal = 'no';
                $in_counter=0;
                $tmp_start_index = $z - $steps;
            }
        }
    }

    # wenn gleitstrecke ohne kurve endet (direkte landung zB)
    if ($in_thermal eq 'no') {
        if (GPLIGCfunctions::dist($DECLAT[$task_finish_time_index], $DECLON[$task_finish_time_index], $DECLAT[$tmp_start_index], $DECLON[$tmp_start_index]) > $min_km) {
            push (@glide_start_time_index, $tmp_start_index);
            push (@glide_end_time_index, $task_finish_time_index);
            push (@glide_head, GPLIGCfunctions::kurs($DECLAT[$tmp_start_index], $DECLON[$tmp_start_index], $DECLAT[$task_finish_time_index], $DECLON[$task_finish_time_index]));
            my $speed = GPLIGCfunctions::dist($DECLAT[$task_finish_time_index], $DECLON[$task_finish_time_index], $DECLAT[$tmp_start_index], $DECLON[$tmp_start_index]) /
              ($DECTIME[$task_finish_time_index]-$DECTIME[$tmp_start_index]);
            push (@glide_speed, $speed);

            push (@glide_dist, GPLIGCfunctions::dist($DECLAT[$task_finish_time_index], $DECLON[$task_finish_time_index], $DECLAT[$tmp_start_index], $DECLON[$tmp_start_index]));

            #print "Glide found !!!!!!! @ $TIME[$tmp_start_index]\n";
        }
    }

    if (Exists($glide_statistik)) {$glide_statistik->destroy;}
    $glide_statistik=$FlightView->Toplevel();
    $glide_statistik->configure(-title=>"Glide");
    setexit($glide_statistik);

    $glide_list_box = $glide_statistik->ScrlListbox(-label=>"Glide", -selectmode=>"single",-height=>"0",-width=>"0")->pack(-side=>"left");
    $glide_list_box->bind("<Double-Button-1>" => \&glide_click);

    my $total_glide_alt = 0;
    my $total_glide_t = 0;
    my $total_glide_dist = 0;
    my $glide_stat_text = '';

    my($ofilename, $odirectories, $osuffix) = File::Basename::fileparse($file);
    $glide_stat_text .= "File: $ofilename\n";

    for (my $z=0; $z<= $#glide_start_time_index; $z++) {
        my $list_zeile='';
        $list_zeile .=  GPLIGCfunctions::time2human($TIME[$glide_start_time_index[$z]],'t')."-".GPLIGCfunctions::time2human($TIME[$glide_end_time_index[$z]],'t')." ";

        my $delta_h = $BARO[$glide_end_time_index[$z]] - $BARO[$glide_start_time_index[$z]];
        $total_glide_alt += $delta_h;
        my $delta_t = $DECTIME[$glide_end_time_index[$z]] - $DECTIME[$glide_start_time_index[$z]];

        $list_zeile .= GPLIGCfunctions::dec2time($delta_t)." ";

        $total_glide_t += $delta_t;
        my $climb_rate = ( ($delta_h/1000) / $delta_t) / 3.6;

        $list_zeile .= sprintf ("  %4.0f $config{'altitude_unit_name'}   %.2f $config{'vertical_speed_unit_name'}  ", $delta_h*$config{'altitude_unit_factor'}, $climb_rate*$config{'vertical_speed_unit_factor'});

        #print $list_zeile."\n";
        $total_glide_dist += $glide_dist[$z];

        $list_zeile .= sprintf ("%.0f $config{'speed_unit_name'}  %.1f $config{'distance_unit_name'}   %.0f", $glide_speed[$z]*$config{'speed_unit_factor'}, $glide_dist[$z]*$config{'distance_unit_factor'}, $glide_head[$z]);

        my $tempdev = ($delta_h / 1000);
        $tempdev=0.00000001 if ($tempdev==0);
        my $gleitzahl =  -1 * $glide_dist[$z] / $tempdev;

        $list_zeile .= sprintf("  %.0f", $gleitzahl);

        $glide_list_box->insert("end", $list_zeile);

    }

    #statistics window:
    if ($total_glide_t != 0) { # if this is 0, there are no glides at all
        my $durchschn_glide_speed = $total_glide_dist / $total_glide_t;
        my $p_tot_gl = sprintf("%.0f",$total_glide_alt*$config{'altitude_unit_factor'});

        $glide_stat_text .= "Total glide height :  $p_tot_gl $config{'altitude_unit_name'}\n";
        $glide_stat_text .= sprintf("Total glide distance : %.2f $config{'distance_unit_name'}\n",$total_glide_dist*$config{'distance_unit_factor'});
        $glide_stat_text .= sprintf("Average glide speed: %.0f $config{'speed_unit_name'}\n", $durchschn_glide_speed*$config{'speed_unit_factor'});

        MessageFenster("Glide statistics", $glide_stat_text);
    } else {
        if (Exists($glide_statistik)) {$glide_statistik->destroy;}
        Errorbox("No glides found");
    }

    sub glide_click {
        my @sel=$glide_list_box->curselection();

        #print "$sel --->".@$sel[0]."<-x- \n";
        #foreach (@$sel) {
        #    print "<-- $_ --> \n";
        #    }
        if (defined $sel[0]) {
            $nr = $glide_start_time_index[$sel[0]];
            FVWausg($glide_start_time_index[$sel[0]]);
        }
    }

}

#####################################

sub MessageFenster {
    my ($head, $message) = @_;

    my $MF=$FlightView->Toplevel();
    $MF->configure(-title=>$head);

    setexit($MF);

    my $label=$MF->Message(-width=>"60c",-textvariable=>\$message);
    $label->pack();

    my $exb=$MF->Button(-text=>"Exit",-command=>sub{$MF->destroy();});
    $exb->pack();
}

##################################

sub save_config {

    print "\nGoing to write the configuration file...\n" if ($config{'DEBUG'});

    if ($^O  ne "MSWin32") {

        # unix systems:
        open (RC, ">$ENV{'HOME'}/.gpligcrc") || die "cannot write configfile";
    } else {

        # windows system, with environmentvariable GPLIGCHOME
        if (defined $ENV{'GPLIGCHOME'}) {

            # if writable
            if ( (-f  "$ENV{'GPLIGCHOME'}\\gpligc.ini" && -w "$ENV{'GPLIGCHOME'}\\gpligc.ini")
                || (!-f "$ENV{'GPLIGCHOME'}\\gpligc.ini" && -w $ENV{'GPLIGCHOME'}) ) {

                open (RC, ">$ENV{'GPLIGCHOME'}\\gpligc.ini") || die "cannot write $ENV{'GPLIGCHOME'}\\gpligc.ini";
            } else {
                Errorbox("Can't write configfile!");
                return;
            }
        } else {
            Errorbox( "GPLIGCHOME Environment variable is not defined. Cannot save config!");
            return;
        }
    }

    print "Writing configuration to file...\n" if ($config{'DEBUG'});

    # write the configfile
    my $count = 0;
    foreach $key (sort(keys %config)) {

        print "$key    \"$config{$key}\" \n" if ($config{'DEBUG'});
        print RC "$key    \"$config{$key}\" \n";
        $count++;
    }

    close RC;

    print "$count parameters written....\n\n" if ($config{'DEBUG'});

}

sub save_gpi {
    my $gpifile = substr($file, 0, -3)."gpi";
    open(GP,">$gpifile") || die "$gpifile cannot be written";

    print "Writing gpi to file...\n" if ($config{'DEBUG'});

    my $count = 0;
    foreach $key (sort(keys %gpi)) {
        print "$key    \"$gpi{$key}\" \n" if ($config{'DEBUG'});
        print GP "$key    \"$gpi{$key}\" \n";
        $count++;
    }

    close GP;
    print "$count parameters written....\n\n" if ($config{'DEBUG'});
}

sub save_tsk {
    my $tskfile = substr($file, 0, -3)."tsk";
    open(GP,">$tskfile") || die "$tskfile cannot be written";

    print "Writing task to file \"$tskfile\"\n" if ($config{'DEBUG'});

    for ($i=1; $i<=$#WPNAME; $i++){

        print "\"$WPNAME[$i]\"  $WPLAT[$i]  $WPLON[$i]  $WPRADFAC[$i]\n" if ($config{'DEBUG'});
        print GP "\"$WPNAME[$i]\"  $WPLAT[$i]  $WPLON[$i]  $WPRADFAC[$i]\n";

    }
    close GP;
}


########################################

# get rid of old config file in 1.10!
sub load_config {

    my $configfile ;
    my $no_tk = shift;

    print "\nGoing to read the configuration file...\n" if ($config{'DEBUG'});

    # determine name and path for configfile

    # unix platforms
    if ($^O ne "MSWin32") {
        $configfile = "$ENV{'HOME'}/.gpligcrc";

        # test old one
        if (!-r $configfile) {
            $configfile = "$ENV{'HOME'}/.GPLIGCrc";
            if (!-r $configfile) {return;}
        }

        # and windows platforms
    } else {

        my $gpligchome;
        if (defined $ENV{'GPLIGCHOME'}) { $gpligchome = $ENV{'GPLIGCHOME'}; }
        else { $gpligchome = ".";}

        $configfile = "$gpligchome\\GPLIGC.ini";

        if (!-r $configfile) {
            $configfile = "$gpligchome\\GPLIGCrc";
            if (!-r $configfile) {return;}
        }

    }

    # open the configfile and check size
    open (RC, "<$configfile") || die "cannot read $configfile";
    my @test = <RC>; close RC;

    print "Size of $configfile is $#test lines \n" if ($config{'DEBUG'});

    # Version 1.4 has 17 options....
    # and since 1.5 we have more
    if ($#test <= 17) {

        # do not use "Errorbox" in windows active perl/Tk  800.024
        my $warn = 1;
        if ($^O eq "MSWin32") {
            if ($Tk::VERSION eq "800.024") { $warn=0;}
        }
        if (defined $no_tk) {$warn=0;}

# strangely this causes hanging main loop on some active perl implementations...?

        if ($warn) {
            Errorbox ("The configuration file ($configfile) is not in the right format for GPLIGC $version \nIt is probably from an older version.\nUse the \"save configuration\"-option to create a new one, or delete the old one.");
        } else {
            print "The configuration file ($configfile) is not in the right format for GPLIGC $version \nIt is probably from an older version.\nUse the \"save configuration\"-option to create a new one, or delete the old one.\n";
        }

        #return;
    }

    open (RC, "<$configfile") || die "cannot open $configfile";
    my @in = ();
    @in = <RC>;
    close RC;

    my $count = 0;
    foreach (@in) {
        if ($_ =~ /(\w+)\s+\"(.*?)\"/) { $count++;}

        print "$1=\"$2\" \n" if ($config{'DEBUG'});
        $config{$1} = $2;
    }
    print "$count parameters read.\n\n" if ($config{'DEBUG'});

    # legacy code to deal with old config files.
    $config{'zylinder_radius'} = 0.2 if ($config{'zylinder_radius'} eq "200m");
    $config{'zylinder_radius'} = 0.3 if ($config{'zylinder_radius'} eq "300m");
    $config{'zylinder_radius'} = 0.4 if ($config{'zylinder_radius'} eq "400m");
    $config{'zylinder_radius'} = 0.5 if ($config{'zylinder_radius'} eq "500m");
    $config{'zylinder_radius'} = 1.0 if ($config{'zylinder_radius'} eq "1km");


}

sub load_gpi {
    my $gpifile = substr($file, 0, -3)."gpi";
    if (-r $gpifile) {open(GP,"<$gpifile") || die "cannot read $gpifile";} else {Errorbox("$gpifile cannot be read"); return;}

    my @in = ();
    @in = <GP>;
    close GP;

    my $count = 0;
    foreach (@in) {
        if ($_ =~ /(\w+)\s+\"(.*?)\"/) { $count++;}

        print "$1= >>$2<< \n" if ($config{'DEBUG'});
        $gpi{$1} = $2;
    }
    print "$count parameters read.\n\n" if ($config{'DEBUG'});
}

########################################


# this sets exit bindings q, esacpe for given widget
sub setexit {

    my $widget = shift;

    $widget->bind("<Key-q>",sub {$widget->destroy();});
    $widget->bind("<Escape>",sub {$widget->destroy();});

}

###########################################

sub optimizer {

    my $contest = shift;
    my $optimizer_cmd;
    my @scoring;

    my $optimizer_cycles;
    $optimizer_cycles = $config{'optimizer_cycles_sa'} if ($config{'optimizer_method'} eq "sa");
    $optimizer_cycles = $config{'optimizer_cycles_mmc'} if ($config{'optimizer_method'} eq "mmc");

    if ($contest eq "olc-class") {  # this is olc-calssic rules 10/2007 see optimizer
        #$optimizer_cmd = "-delta 1000 -w 7 -p 1 1 1 1 0.8 0.6";
        $optimizer_cmd = "-olc-class -w 7 -p 1 1 1 1 0.8 0.6";
        @scoring = (1, 1, 1, 1, 0.8, 0.6);

        #print "$#scoring \n";
    }

    if ($contest eq "olc-fai") {  # this is olc-calssic rules 10/2007 see optimizer
        #$optimizer_cmd = "-delta 1000 -w 7 -p 1 1 1 1 0.8 0.6";
        $optimizer_cmd = "-olc-fai -w 5 -p 1 1 1 1";
        @scoring = (1, 1, 1, 1);

        #print "$#scoring \n";
    }

    if ($contest eq "dmst") { # rules 2005 ?, see optimizer
        $optimizer_cmd = "-dmst -w 5 -p 1 1 1 1";
        @scoring = (1, 1, 1, 1);

        #print "$#scoring \n";
    }

    if ($contest eq "olc-phg"){ # rules 3/2008 ? see optimizer
        $optimizer_cmd = "-holc -w 5 -p 1.5 1.5 1.5 1.5";
        @scoring = (1.5, 1.5, 1.5, 1.5);
    }

    if ($config{'optimizer_debug'}) {
        $optimizer_cmd .= " -debug ";
    }

    if ($config{'optimizer_verbose'}) {
        $optimizer_cmd .= " -v ";
    }

    #check if optimizer is in path:
    if (system("optimizer")) {Errorbox("Can't execute the \"optimizer\". Check installation.");
        return;
    }

    $optimizer_cmd .= " $config{'optimizer_mmc'} " if ($config{'optimizer_method'} eq "mmc");
    $optimizer_cmd .= " $config{'optimizer_sa'} " if ($config{'optimizer_method'} eq "sa");

    $busy = $FlightView->Busy;
    $busy2 = $WPPlotWindow->Busy;

    #$busy_fvw = $FlightView->Busy if (Exists($FlightView));

    if ($contest eq "olc-phg"){
        if ($BARO[$task_start_time_index] - $BARO[0] > 762.195) { # 2500ft
            my $delta = ($BARO[$task_start_time_index]-$BARO[0])*$config{'altitude_unit_factor'};
            Errorbox("WARNING: Altitude difference between take-off location and release point: "
                  . "$delta $config{'altitude_unit_name'}. This violates 4.5 in DHV Online contest Rules for 2005 (Rel. 1.5)".
                  " in the case you were towed by an UL. According to 4.5 winch launching seems to be OK.");
        }
    }

    #progress bar!
    require Tk::ProgressBar;
    my $progresswindow = $FlightView->Toplevel();
    $progresswindow->configure(-title=>"Optimization progress");
    my $leer;

    # using the -variable is a workaround for some small bug in ProgressBar
    my $progress = $progresswindow -> ProgressBar(-width=>20, -length=>400, -blocks=>1,
        -anchor=>'w',-from=>0,-to=>$optimizer_cycles,-variable=>\$leer, -colors=>[0, "blue"],- troughcolor=>"grey") ->pack;

    my @optwpindex=();
    my @Points=();
    my @Dist=();
    for (my $t=0; $t<=$optimizer_cycles; $t++) {					##########TO CHANGE NUMBER OF CYCLES######!!!
        $optwpindex[$i]=GPLIGCfunctions::retArrayRef();
    }

    for (my $i=0 ; $i<=$optimizer_cycles; $i++){					#########################################!!!
        my $seed = int (rand 1000000)+1;
        my $optimizeout = `optimizer  $optimizer_cmd -c 1 -f $task_start_time_index -l $task_finish_time_index  -d $pfadftmp/3d.dat -s $seed`;

        if ($config{'DEBUG'}) {
            print "optimizer $optimizer_cmd -c 1 -f $task_start_time_index -l $task_finish_time_index  -d $pfadftmp/3d.dat -s $seed\n";
            print "$optimizeout";
        }

        if ($#scoring == 5) {
            $optimizeout =~ /\s*(\d+)\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/;
            $MAXWP = $1;
            $Points[$i] = $3;
            $Dist[$i] = $2;
            @{$optwpindex[$i]} = ($4, $5, $6, $7, $8,  $9, $10);
        }

        if ($#scoring == 2) {
            $optimizeout =~ /\s*(\d+)\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/;
            $MAXWP = $1;
            $Points[$i] = $3;
            $Dist[$i] = $2;

            #print "$MAXWP \n";
            @{$optwpindex[$i]} = ($4, $5, $6, $7);
        }

        if ($#scoring == 3) {

            #print "$optimizeout \n";
            $optimizeout =~ /\s*(\d+)\s+(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/;
            $MAXWP = $1;
            $Points[$i] = $3;
            $Dist[$i] = $2;

            #print "$MAXWP \n";
            @{$optwpindex[$i]} = ($4, $5, $6, $7, $8);

            #print "$2, $3, $4, $5, $6, $7, $8 \n";
        }

        $progress->value($i);
        $progresswindow->update;

        #print "cycle $i  punkte  $Points[$i] \n";

    }

    $progresswindow->destroy();

    my $entf=0;
    my $pkt=0;
    my $punkteMax=0;
    my $punkteMaxIndex;

    for (my $j=0; $j<=$optimizer_cycles; $j++) {
        $pkt=0;
        $pkt = $Points[$j];

        if ($pkt > $punkteMax) {
            $punkteMax= $pkt;
            $punkteMaxIndex=$j;
        }
    }

    # put optimized task in task variables
    @WPNAME=();
    @WPLAT=();
    @WPLON=();
    for (my $i=1; $i<=$MAXWP; $i++) {
        $WPNAME[$i]="WP".($i);
        $WPLAT[$i]=$DECLAT[${$optwpindex[$punkteMaxIndex]}[$i-1]];
        $WPLON[$i]=$DECLON[${$optwpindex[$punkteMaxIndex]}[$i-1]];
    }

    # needed for calculation of Abflugzeit und Wertundsendezeit
    $olcLastWPindex=${$optwpindex[$punkteMaxIndex]}[6] if ($contest eq "olc-class");
    $olcLastWPindex=${$optwpindex[$punkteMaxIndex]}[4] if ($contest eq "dmst");
    $olcLastWPindex=${$optwpindex[$punkteMaxIndex]}[4] if ($contest eq "olc-phg");
    $olcFirstWPindex=${$optwpindex[$punkteMaxIndex]}[0];

    $optimized_for = $contest;

    WPPlotUpdate();

    $entf = sprintf ("%4.2f",$Dist[$punkteMaxIndex]);
    $pkt  = sprintf ("%4.2f",$Points[$punkteMaxIndex]);

    my ($mess2) = olctaskinfo();
    my $mess  = "Optimization ($contest) result: $entf km  $pkt points\n\n".$mess2;

    MessageFenster ("Optimization Results", $mess);

    $busy = $FlightView->Unbusy;
    $busy2 = $WPPlotWindow->Unbusy;

    #$busy_fvw = $FlightView->Unbusy if (Exists($FlightView));

    FSupdate();
    TaskUpdate();
    baroplot();#_task();

}

######### END OPTIMIZER ##################################


sub getWPreachedIndices {
    my @wpindex = ();
    my @wpnames = ();

    if ($MAXWP > 0) {
        my $loopstart=1;
        for (my $z=1; $z <= $MAXWP; $z++) {

        my $limit = $config{'zylinder_radius'};

            for (my $x=$loopstart; $x <= $#DECLAT; $x++) {
                my $dist = GPLIGCfunctions::dist($DECLAT[$x], $DECLON[$x], $WPLAT[$z], $WPLON[$z]);
                if ($dist <= $limit) {
                    push(@wpindex, $x);
                    push(@wpnames, $WPNAME[$z]);
                    $loopstart=$x;
                    print "reached WP: $WPNAME[$z], $x, $dist  \n" if ($config{'DEBUG'});
                    last;
                }
            }

        }
    }

    return (\@wpindex, \@wpnames);
}

# open given url in a browser
sub browseURL {

    print "browseURL called with following URL:\n" if ($config{'DEBUG'});

    my $url = shift;

    print ">>>$url<<<\n" if ($config{'DEBUG'});

    # unix-case
    if ($^O ne 'MSWin32') {

        # browser is a user option
        # $browser = "/usr/local/mozilla/mozilla"; # now in $config

        print "This is NOT win32, config(browser) = >>$config{'browser'}<<\n" if ($config{'DEBUG'});

        my $intbrows = "none";

        if (-x $config{'browser'} && -f $config{'browser'}) {
            $intbrows = $config{'browser'};
            print "$config{'browser'} is OK\n" if ($config{'DEBUG'});

        } else {

            print "$config{'browser'} is NOT OK. We will try to find one of netscape, mozilla or firefox in std locations.\n" if ($config{'DEBUG'});

            # try to find a browser
            @browsbins = qw(
              /usr/local/firefox/firefox
              /usr/local/bin/firefox
              /usr/bin/firefox
              /opt/firefox/firefox
              /usr/local/mozilla/mozilla
              /usr/local/bin/mozilla
              /usr/bin/mozilla
              /opt/mozilla/mozilla
              /usr/local/netscape/netscape
              /usr/local/bin/netscape
              /usr/bin/netscape
              /opt/netscape/netscape
              );

            @binpaths = qw(/bin /usr/bin /usr/local/bin);
            @binnames = qw(/firefox /mozilla /firebird /MozillaFirebird /netscape);

            @installpaths = qw(/usr /usr/local /opt);

            @bins2 = qw(	/firefox/firefox
              /mozilla/mozilla
              /netscape/netscape
              /firebird/firebird
              /MozillaFirebird/MozillaFirebird
              );

            foreach my $b (@browsbins) {
                if (-x $b && -f $b) {
                    $intbrows = $b;
                    print "$b is the browser!\n" if ($config{'DEBUG'});
                }
            }

            foreach my $p1 (@binpaths) {
                foreach my $p2 (@binnames) {
                    if (-x $p1.$p2 && -f $p1.$p2) {
                        $intbrows = $p1.$p2;
                        print "$p1$p2 is the browser!\n" if ($config{'DEBUG'});
                    }

                }
            }

            foreach my $p1 (@installpaths) {
                foreach my $p2 (@bins2) {
                    if (-x $p1.$p2 && -f $p1.$p2) {
                        $intbrows = $p1.$p2;
                        print "$p1$p2 is the browser!\n" if ($config{'DEBUG'});
                    }

                }
            }

        }

        if ($^O eq "darwin" && $intbrows eq "none") {

            $intbrows = "open";

            if ($config{'DEBUG'}) {
                print "Operating system is darwin and we didnt find any special browser...! We will use the open command to start browser!\n";

            }

        }

        if ($intbrows eq "none") {Errorbox("No browser available!"); return;}

        # open URL on unix

        # Mac OS X (darwin)
        if ($^O eq "darwin" && $intbrows eq "open") {
            my $ret = system ("$intbrows \"$url\" &");
            print "$intbrows \"$url\" \n" if ($config{'DEBUG'});
            print "return value of system .. open is >>>$ret<<<\n" if ($config{'DEBUG'});
        } else {

            print "$intbrows -remote \"openURL($url,new-window)\" \n" if ($config{'DEBUG'});

            if (system("$intbrows -remote \"openURL($url,new-window)\"") != 0) {
                print "couldn't start new browser window remotely\n" if ($config{'DEBUG'});
                if (system("$intbrows \"$url\" &") != 0) {
                    print "opening of browser failed\n" if ($config{'DEBUG'});
                    Errorbox("Error while trying to start $intbrows");
                }
            } else {
                print "opened a new $intbrows window!\n" if ($config{'DEBUG'});
            }
        }

    } else {

        # windows methods
        system("rundll32 url.dll,FileProtocolHandler \"$url\"");
    }

}

################################################################################

sub checkPhotos {
    my $flag_from_open_photos = shift;

    ## this initializes photo functionality
    ## reads photos via getPhotos
    ## and gets times via GPLIGCfunctions::getJPEGtime

    @PHOTO_tmp=();
    @PHOTO_TIMES_tmp=();
    $delta_photo_time = 0 ;

    # Photo stuff
    # initialization

    if ($config{'photos'}) {

        @PHOTO_tmp = getPhotos(File::Basename::dirname($file)) if (!$flag_from_open_photos);
        print "$#PHOTO_tmp   No. of Photos/Multimedia files\n" if ($config{'DEBUG'});

        if ($#PHOTO_tmp < 0) {
            if ($config{'photo_path'} ne "none") {
                @PHOTO_tmp = getPhotos($config{'photo_path'});
            } else {
                @PHOTO_tmp = getPhotos(File::Basename::dirname($file));
            }
        }

        # Process
        if ($#PHOTO_tmp >= 0) {

            my $time;
            foreach (@PHOTO_tmp) {

                $time=0;

                if ($have_EXIF && (lc(substr($_,-3)) eq "jpg" || lc(substr($_,-4)) eq "jpeg") ) {
                    $time = GPLIGCfunctions::getJPEGtime($_);
                }

                #print "File >$_<\n";
                if ($time == 0) {

                    #print "get from $_ \n";
                    $time = GPLIGCfunctions::getFiletime($_);
                }

                push (@PHOTO_TIMES_tmp, $time-$photo_timezone);

                if ($config{'DEBUG'}){
                    print "$_ : $time - ($photo_timezone) \n";
                }

            }

        }

    }
}

################################################################################

sub processPhotos {
    @PHOTO_FILES=();
    @PHOTO_FILES_INDICES=();
    @PHOTO_FILES_TIMES=();	#

    # photo analyzing II
    # now we create array PHOTO_FILES an _INDICES and TIMES
    if ($config{'photos'} && $#PHOTO_TIMES_tmp >= 0) {
        for (my $z=0; $z<=$#PHOTO_tmp; $z++) {

            print "." if ($config{'DEBUG'});

            my $min = 0.0166666;
            my $min_idx = 0;
            my $ptime;
            my $comptime;
            for (my $x=0; $x<=$#DECTIME; $x++) {

                #print "checking.... $x\n";

                $ptime = $PHOTO_TIMES_tmp[$z]-$delta_photo_time;
                $comptime = $DECTIME[$x];
                while ($ptime < 0) {$ptime+=24;}
                while ($ptime > 24) {$ptime-=24;}
                while ($comptime < 0) {$comptime+=24;}
                while ($comptime > 24) {$comptime-=24;}

                if ( abs($comptime-$ptime) < $min ) {
                    $min = abs($comptime-$ptime);
                    $min_idx = $x;

                }

            }

            if ($min < 0.016666) {
                push (@PHOTO_FILES, $PHOTO_tmp[$z]);
                push (@PHOTO_FILES_INDICES, $min_idx);
                push (@PHOTO_FILES_TIMES, $ptime);
            }

        }

        if ($config{'DEBUG'}){
            print "These Photos/MM are close to track (in time):\n";
            for (my $z=0; $z<=$#PHOTO_FILES; $z++) {
                print "$PHOTO_FILES[$z] ---> $PHOTO_FILES_INDICES[$z] <-- $PHOTO_FILES_TIMES[$z]\n";
            }
        }

    }

}

################################################################################

sub getPhotos {

    ## This reads jpeg filenames from given dir
    ## and returns array

    my $dir = shift;
    my @jpegs = ();

    # will be overridden by GPLIGC-timeshift file
    $photo_timezone = $gpi{'timezone'};

    print "trying to find photos/multimedia in $dir\n" if ($config{'DEBUG'});

    opendir (PHOTODIR, $dir) || die "Can not read $dir";
    my @content = sort readdir(PHOTODIR);
    closedir (PHOTODIR);

    foreach (@content) {

         if ( lc(substr($_,-3)) eq "jpg" || lc(substr($_,-4)) eq "jpeg"
            || lc(substr($_,-3)) eq "amr" || lc(substr($_,-3)) eq "ogg" || lc(substr($_,-3)) eq "mp3"
            || lc(substr($_,-3)) eq "avi" || lc(substr($_,-3)) eq "3gp"
          ) {

            push (@jpegs, "$dir/$_");
            print "$dir/$_<=== \n" if ($config{'DEBUG'});

        }

    }

    if ($config{'DEBUG'}) {
        print ($#jpegs+1);
        print " pictures/mm (array+1) found!\n";
    }

    # this file overrides timezone from .GPLIGCrc or .gpi file
    if (-r "$dir/.GPLIGC-timeshift") {
        open (TS, "<$dir/.GPLIGC-timeshift") || die "Cant read $dir/.GPLIGC-timeshift";
        chomp($delta_photo_time = <TS>);
        chomp($photo_timezone = <TS>);
        close TS;

        print "read $dir/.GPLIGC-timeshift:  $delta_photo_time \n" if ($config{'DEBUG'});
        print "read $dir/.GPLIGC-timeshift (tz):  $photo_timezone \n" if ($config{'DEBUG'});
    }

    return @jpegs;
}

################################################################################

sub showMM {

    my $file = shift;

    print "Showing/Playing: $file \n" if ($config{'DEBUG'});

    if ( lc(substr($file,-3)) eq "jpg" || lc(substr($file,-4)) eq "jpeg") {
        if ($config{'picture_viewer'} eq "internal") {
            viewPhoto($file);
        } else {
            system ( " $config{'picture_viewer'} $file  2>/dev/null >&2  &");
        }
    }

    if (lc(substr($file,-3)) eq "avi" || lc(substr($file,-3)) eq "amr"
        || lc(substr($file,-3)) eq "mp3" || lc(substr($file,-3)) eq "ogg"
        || lc(substr($file,-3)) eq "3gp" || lc(substr($file,-3)) eq "wav"

      ) {

        system ( "$config{'mm_player'} $file  2>/dev/null >&2 &");

    }

}

sub viewPhoto {

    #this is our internal photo viewer

    my $file = shift;

    if ($config{'DEBUG'}) {
        print "$file will be viewed...\n";
    }

    if (!$have_jpeg) {
        Errorbox ("Sorry. No Tk::JPEG module avaiable");
        return;
    }

    my $viewer=$FlightView->Toplevel();
    $viewer->configure(-title=>"GPLIGC: $file");

    my $canvas = $viewer->Canvas();
    my $photo = $viewer->Photo();
    $canvas->create('image', 0, 0, -image=>$photo, -anchor =>'nw');
    my $yscroll = $viewer->Scrollbar(-command=>['yview',$canvas],-orient=>'vertical');
    my $xscroll = $viewer->Scrollbar(-command=>['xview',$canvas],-orient=>'horizontal');

    $canvas ->configure(-xscrollcommand=>['set', $xscroll], -yscrollcommand=>['set', $yscroll]);

    $photo->configure(-file=>"$file");
    $canvas->configure(-scrollregion=>[0,0,$photo->width,$photo->height],-width=>$photo->width,-height=>$photo->height);

    $xscroll->pack(-side=>'bottom', -fill =>'x'	);
    $yscroll->pack(-side=>'right', -fill =>'y'	);

    $canvas->pack(-expand=>'yes', -fill=>'both',-anchor=>'s');

    setexit($viewer);
}

sub geotag_photos {

    if (!$config{'photos'}) {return;}
    if (!have_EXIF) {Errorbox("Cannot do that without the Image::ExifTool module. Sorry!"); return;}
    if ( $#PHOTO_FILES < 1 ) {Errorbox("No photos available to do that!");return;}

    my $processed = 0;
    my $err = 0;
    my $rej_overwrite = 0;

    my $resp=$FlightView->messageBox(-type=>'OkCancel', -default=>'Ok', -icon=>'question',
        -message=>"This will geo-tag all jpeg's of the media list (".($#PHOTO_FILES+1)." files)");

    if (lc($resp) eq 'cancel') {return;}

    # check for jpeg...
    # give list and option to cancel here!

    $busy2 = $FlightView->Busy;

    for (my $i = 0; $i<=$#PHOTO_FILES; $i++) {

        if ($config{'DEBUG'}) {
            print "Tag: $PHOTO_FILES[$i]  $DECLAT[$PHOTO_FILES_INDICES[$i]] $DECLON[$PHOTO_FILES_INDICES[$i]] $BARO[$PHOTO_FILES_INDICES[$i]] \n";
        }

        # we should count files and intercept errors.
        if ( lc(substr($PHOTO_FILES[$i],-3)) eq "jpg" || lc(substr($PHOTO_FILES[$i],-4)) eq "jpeg") {

            my $ret = GPLIGCfunctions::geotag( $PHOTO_FILES[$i], $DECLAT[$PHOTO_FILES_INDICES[$i]], $DECLON[$PHOTO_FILES_INDICES[$i]], $BARO[$PHOTO_FILES_INDICES[$i]],
                $DECTIME[$PHOTO_FILES_INDICES[$i]], $gpi{'timezone'}, $config{'geotag_force_overwrite'} );

            if ( $ret == 1) { $processed += 1; }
            if ( $ret == 2) { $rej_overwrite +=1; }
            if ( $ret == 0) { $err +=1; }

        } else {print "$PHOTO_FILES[$i] is not a jpeg\n" if ($config{'DEBUG'});}
    }
    $busy2 = $FlightView->Unbusy;
    Errorbox("".($#PHOTO_FILES+1)." files. Processed: $processed  Errors: $err  Rejected to overwrite tags: $rej_overwrite");
}

###############################################################################

sub phototimecalibration {

    if (!defined $lastphoto_idx) {
        Errorbox("Please view a photo first, subsequently you can enter the exact time for that photo.");
        return;
    }

    my $input=$FlightView->Toplevel();

    $input->configure(-title=>"Photo time calibration");

    my $eleframe=$input->Frame()->pack();
    my $buttonfr=$input->Frame()->pack();
    my $extime = "$TIME[$nr]";
    my ($name, $p, $suf) = File::Basename::fileparse($PHOTO_FILES[$lastphoto_idx]);

    $eleframe->Label(-text=> "Exact time of last photo seen ($name) UTC (hh:mm:ss or hhmmss)")->pack(-side=>"left");
    $eleframe->Entry(-textvariable=>\$extime)->pack(-side=>"right");

    $buttonfr->Button(-text=>"OK",-command=>sub{
            $input->destroy();
            return;
          })->pack(-side=>"left");
    $buttonfr->Button(-text=>"cancel",-command=>sub{
            $input->destroy();
            undef $extime;
            return;
          })->pack(-side=>"right");

    $input -> waitWindow;

    ###### waiting?

    if (!defined $extime) {
        Errorbox ("No input given / cancelled");
        return;
    }

    # check format
    if (! ( $extime =~ /(\d{2}):(\d{2}):(\d{2})/ || $extime =~ /(\d{6})/ ) ) {
        Errorbox ("Input not in required format");
        return;
    }

    $extime =~s/://g;  #strip ":" if existing
    my $dectime = GPLIGCfunctions::time2dec($extime);
    print "given:  $extime $dectime\n" if ($config{'DEBUG'});
    print "last photo: $PHOTO_FILES_TIMES[$lastphoto_idx] \n" if ($config{'DEBUG'});

    $delta_photo_time = $PHOTO_FILES_TIMES[$lastphoto_idx] - $dectime;
    print "Difference:  $delta_photo_time   to be applied to all photos!\n" if ($config{'DEBUG'});

    for (my $x=0 ; $x <= $#PHOTO_FILES_TIMES; $x++) {
        print "$PHOTO_FILES_TIMES[$x]   -----> ".($PHOTO_FILES_TIMES[$x]-$delta_photo_time)."\n" if ($config{'DEBUG'});

        #	print "$PHOTO_TIMES[$x]  \n " if ($config{'DEBUG'});
    }

    $p .= "/.GPLIGC-timeshift";

    #    if (!-w $p) {
    #	Errorbox ("Sorry couldn't write timeshiftfile: $p");
    #    } else {
    open (TS, ">$p") || die "couldn't write timeshift-file: $p";
    print TS $delta_photo_time."\n";
    print TS $photo_timezone."\n";
    close TS;

    #    }

    processPhotos();

}

################################################# KML EXPORT
sub kml_export {
    if ($filestat eq 'closed') {$FlightView->bell; return;}
    $kml_file = substr($file,0,-3)."kml";

    print "kml file will be written to $kml_file\n" if ($config{'DEBUG'});
    open (KML, ">$kml_file") || die "Cannot write $kml_file";

    $kml_segment=1;
    print KML "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://earth.google.com/kml/2.1\">\n<Document>\n";
    print KML "\n <Placemark>\n  <name>$kml_segment</name>\n   <LineString>\n    <altitudeMode>absolute</altitudeMode>\n    <coordinates>\n";

    for ($z=1; $z<=$#DECLAT; $z++) {
        print KML "       $DECLON[$z],$DECLAT[$z],$BARO[$z]\n";
    }

    print KML "    </coordinates>\n   </LineString>\n </Placemark>\n";
    print KML "\n</Document>\n</kml>\n";
    close KML;
    MessageFenster("kml export","kml file was written to $kml_file");
}

########################################### GPX EXPORT
sub gpx_export {
    if ($filestat eq 'closed') {$FlightView->bell; return;}
    my $gpx_file = substr($file,0,-3)."gpx";

    print "gpx file will be written to $gpx_file\n" if ($config{'DEBUG'});
    open (GPX, ">$gpx_file") || die "Cannot write $gpx_file";

    print GPX "<?xml version=\"1.0\"?>\n<gpx version=\"1.0\" creator=\"GPLIGC $version\">\n";

    print GPX "<time></time>\n";
    print GPX "<trk>\n <name>$file</name>\n   <trkseg>\n";

    my $fakedate="1900-01-01";
    my $ftime;
    for ($z=1; $z<=$#DECLAT; $z++) {
        print GPX "    <trkpt lat=\"$DECLAT[$z]\" lon=\"$DECLON[$z]\">\n";
        $ftime = GPLIGCfunctions::time2human($TIME[$z],'t');
        print GPX "     <ele>$BARO[$z]</ele>\n";
        print GPX "     <time>${fakedate}T${ftime}Z</time>\n";
        print GPX "    </trkpt>\n";
    }

    print GPX "   </trkseg>\n</trk>\n</gpx>\n";

    close GPX;
    MessageFenster("gpx export","gpx file was written to $gpx_file");
}
##################################################
sub download_media {
    if ($^O eq "MSWin32") {Errorbox("not supported on windows!"); return;};
    my $dir= directory();
    if (-d $dir) {
        my $com = "mount -v $config{'mm_mountpoint'}  ";

        foreach ( $config{'mm_download_dirs'} =~ /(\w+)/g ) {
            $com .= " ; cp -av $config{'mm_mountpoint'}/$_/* $dir ";
        }

        $com .= " ; umount -v $config{'mm_mountpoint'}  ";

        print "Media DL command: $com \n" if ($config{'DEBUG'});

        my $pid = fork();

        if ($pid==0) {
            system ("$config{'terminal'} \'$com\'");
            wait();
            CORE::exit();
        }
    } else {Errorbox("No direcotry chosen");}
}

#############################################################
sub download_garmin {
    if ($^O eq "MSWin32") {Errorbox("not supported on windows!"); return;};
    my $file = save(GPLIGCfunctions::get_name_date()."-GAR-000-01.gpspoint");
    my $path = File::Basename::dirname($file);

    if ($file ne "no") {

        my $com = " $config{'garmin_download'}  >$file && cd $path && gpsp2igcfile.pl <$file && echo \"finished\"";
        print "Garmin download command: $com \n" if ($config{'DEBUG'});

        my $pid = fork();
        if ($pid==0) {
            system ("$config{'terminal'} \'$com\'");
            wait();
            CORE::exit();
        }
    }
}

sub download_gpsbabel {

    # needs testing on windows
    #if ($^O eq "MSWin32") {Errorbox("not supported on windows!"); return;};
    #my $num=0;
    #startover:

    my $file = save(GPLIGCfunctions::get_name_date()."-XXX-000-01.igc");
    my $path = File::Basename::dirname($file);

    if (-e $file) {
        Errorbox("File exists ($file)");
        return;	}

    if ($file ne "no") {

        my $com = " $config{'gpsbabel_tdownload'} $file && echo \"finished\"";

        #print "$com \n";

        if ($^O ne "MSWin32") {
            my $pid = fork();
            if ($pid==0) {
                system ("$config{'terminal'} \'$com\'");
                wait();
                CORE::exit();
            }
        } else {

            # windows code:
            system ("start $com");

        }
    }
}

#####

sub gpiinput {

    if ($filestat eq 'closed') {$FlightView->bell; return;}
    my $input=$FlightView->Toplevel();

    $input->configure(-title=>"Additional flight information");

    my %oldgpi = %gpi;

    my $fr1=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr2=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr3=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr4=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr5=$input->Frame()->pack(-expand=>"yes",-fill=>"x");

    #my $fr5b=$input->Frame()->pack;
    my $fr6=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr7=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr8=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr9=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr10=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr11=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr12=$input->Frame()->pack(-expand=>"yes",-fill=>"x");
    my $fr13=$input->Frame()->pack(-expand=>"yes",-fill=>"x");

    my $bfr=$input->Frame()->pack();

    $fr1->Label(-text=> "Date of flight")->pack(-side=>"left");
    $fr1->Entry(-textvariable=>\$gpi{'year'})->pack(-expand=>"yes",-fill=>"x");
    $fr1->Entry(-textvariable=>\$gpi{'month'})->pack(-expand=>"yes",-fill=>"x");
    $fr1->Entry(-textvariable=>\$gpi{'day'})->pack(-expand=>"yes",-fill=>"x");

    $fr2->Label(-text=> "Pilot",-width=>"8")->pack(-side=>"left",-anchor=>"w");
    $fr2->Entry(-textvariable=>\$gpi{'pilot'})->pack(-expand=>"yes",-fill=>"x");

    $fr3->Label(-text=> "Copilot",-width=>"8")->pack(-side=>"left");
    $fr3->Entry(-textvariable=>\$gpi{'copilot'})->pack(-expand=>"yes",-fill=>"x");

    $fr4->Label(-text=> "Glider",-width=>"8")->pack(-side=>"left");
    $fr4->Entry(-textvariable=>\$gpi{'glider'})->pack(-expand=>"yes",-fill=>"x");

    $fr5->Label(-text=> "Callsign",-width=>"8")->pack(-side=>"left");
    $fr5->Entry(-textvariable=>\$gpi{'callsign'})->pack(-expand=>"yes",-fill=>"x");

    #$fr5b->Label(-text=> "Glider mass [kg]")->pack(-side=>"left");
    #$fr5b->Entry(-textvariable=>\$gpi{'glider_mass'})->pack();

    $fr6->Label(-text=> "QNH (hPa)")->pack(-side=>"left");
    $fr6->Entry(-textvariable=>\$gpi{'qnh'})->pack(-expand=>"yes",-fill=>"x");

    $fr7->Label(-text=> "Timezone (offset from UTC)")->pack(-side=>"left");
    $fr7->Entry(-textvariable=>\$gpi{'timezone'})->pack(-expand=>"yes",-fill=>"x");

    $fr8->Label(-text=> "Airfield")->pack(-side=>"left");
    $fr8->Entry(-textvariable=>\$gpi{'airfield'})->pack(-expand=>"yes",-fill=>"x");

    $fr9->Label(-text=> "Altitude calibration applied: $gpi{'altitude_calibration_type'} (ca. ".int($offset).")")->pack(-side=>"left");
    $fr9->Button(-text=>"Clear calibration",-command=>sub{
            $gpi{'altitude_calibration_ref_press'}=0;
            $gpi{'altitude_calibration_type'}="none";
            $gpi{'altitude_calibration_point'}=0;
            $gpi{'altitude_calibration_point_alt'}=0;
            $offset = 0;
            Errorbox("To make changes take effect, save information and reload igc-file!");
          })->pack(-expand=>"yes",-fill=>"x");

    $fr10->Label(-text=> "Logger Device")->pack(-side=>"left");
    $fr10->Entry(-textvariable=>\$gpi{'logger'})->pack(-expand=>"yes",-fill=>"x");
    $fr11->Label(-text=> "Remarks1")->pack(-side=>"left");
    $fr11->Entry(-textvariable=>\$gpi{'remarks'})->pack(-expand=>"yes",-fill=>"x");
    $fr12->Label(-text=> "Remarks2")->pack(-side=>"left");
    $fr12->Entry(-textvariable=>\$gpi{'remarks2'})->pack(-expand=>"yes",-fill=>"x");
    $fr13->Label(-text=> "Remarks3")->pack(-side=>"left");
    $fr13->Entry(-textvariable=>\$gpi{'remarks3'})->pack(-expand=>"yes",-fill=>"x");

    $bfr->Button(-text=>"Save",-command=>sub{
            save_gpi();
            $input->destroy();
            return;
          })->pack(-side=>"left");
    $bfr->Button(-text=>"OK",-command=>sub{
            $input->destroy();

            # check for changes? offset correction?!
            return;
          })->pack(-side=>"left");
    $bfr->Button(-text=>"cancel",-command=>sub{
            $input->destroy();
            %gpi = %oldgpi;
            return;
          })->pack(-side=>"right");

    $input -> waitWindow;
}


sub check_config {
  # find outdated config-keys and inform user
  print "\ncheck_config\n" if ($config{'DEBUG'});
  my $YES = 0;
  my $text = "The following configuration keys are outdated. They will be deleted now. Save the configuration to erase them permanently from your configuration file.\nKeys:\n";

  my @old_keys = qw (optimizer_montecarlo optimizer_metropolis_montecarlo optimizer_cycles open_flight_view_window fvw_show_help
  flight_view_baro_grid flight_view_window_grid datadir libdir
  );
  my $oc;

  foreach $oc (@old_keys) {
    print "checking:  $oc" if ($config{'DEBUG'});
    if (exists ($config{$oc})) {
      print "=$config{$oc}  FOUND\n" if ($config{'DEBUG'});
      delete ( $config{$oc});
      $YES = 1;
      $text .= "$oc ";
    } else {
      print "\n" if ($config{'DEBUG'});
    }
  }

  if ($YES) {Errorbox($text);}
}


sub olctaskinfo {

    # departure time (olc) s0
    # finishing time (olc) s6
    my $starttime;
    my $finishtime;

    my $starttimeindex=GPLIGCfunctions::takeoff_detect(\@nonISPEED, 's');
    my $landtimeindex=GPLIGCfunctions::takeoff_detect(\@nonISPEED, 'l');

    # take-off time (olc) t0
    my $stime=GPLIGCfunctions::time2human($TIME[$starttimeindex], 't');
    my $ltime=GPLIGCfunctions::time2human($TIME[$landtimeindex], 't');

    # unpowered filght start, end (olc) fstart ffinish
    my $fstart=GPLIGCfunctions::time2human($TIME[$task_start_time_index],'t');
    my $ffinish=GPLIGCfunctions::time2human($TIME[$task_finish_time_index],'t');

    my $maximumheightindex;
    my $minimumheightindex;

    if ($optimized_for eq "olc-class") {

        # <should be impossible. but = may happen
        if ($olcFirstWPindex <= $task_start_time_index) { $minimumheightindex=$olcFirstWPindex; $starttime = GPLIGCfunctions::time2human($TIME[$olcFirstWPindex],'t'); }
        else {

            # get lowest point between task_start_time_index and olcFirstWPindex
            $minimumheightindex = $olcFirstWPindex;

            for (my $e=$task_start_time_index; $e <= $olcFirstWPindex; $e++) {
                if ($BARO[$e] < $BARO[$minimumheightindex]) {$minimumheightindex = $e;}
            }

            $starttime = GPLIGCfunctions::time2human($TIME[$minimumheightindex],'t');

        }

        # > should be impossible. but = may happen
        if ($olcLastWPindex >= $task_finish_time_index) { $maximumheightindex=$olcLastWPindex; $finishtime = GPLIGCfunctions::time2human($TIME[$olcLastWPindex],'t'); }
        else {

           # get highest point between task_finish_time_index and olcLastWPindex
            $maximumheightindex = $olcLastWPindex;

            for (my $e=$olcLastWPindex; $e <= $task_finish_time_index; $e++) {
                if ($BARO[$e] > $BARO[$maximumheightindex]) {$maximumheightindex = $e;}
            }

            $finishtime = GPLIGCfunctions::time2human($TIME[$maximumheightindex],'t');
        }
    }

    if ($optimized_for eq "olc-phg") {
        $finishtime = GPLIGCfunctions::time2human($TIME[$olcLastWPindex],'t');
        $starttime = GPLIGCfunctions::time2human($TIME[$olcFirstWPindex],'t')
    }

    if ($optimized_for eq "dmst") {
        $finishtime = GPLIGCfunctions::time2human($TIME[$olcLastWPindex],'t');

        $starttime = GPLIGCfunctions::time2human($TIME[$olcFirstWPindex],'t') if ($BARO[$task_start_time_index] > $BARO[$olcFirstWPindex]);
        $starttime = GPLIGCfunctions::time2human($TIME[$task_start_time_index],'t') if ($BARO[$task_start_time_index] < $BARO[$olcFirstWPindex]);

    }

    my $message_out = "";
    $message_out .= "Take-Off time (t0) = ".$stime."\n";
    $message_out .= "Begin of unpowered flight (fstart) = ".GPLIGCfunctions::time2human($TIME[$task_start_time_index],'t')."\n";
    $message_out .= "Time of departure (s0)  = $starttime\n" if ($optimized_for ne "none");

    $message_out .= "first WP = ".GPLIGCfunctions::time2human($TIME[$olcFirstWPindex],'t')."\n" if (defined $olcFirstWPindex);
    $message_out .= "last  WP = ".GPLIGCfunctions::time2human($TIME[$olcLastWPindex],'t')."\n" if (defined $olcLastWPindex);

    $message_out .= "finishtime (s6/s4) = $finishtime\n" if ($optimized_for ne "none");
    $message_out .= "End of unpowered flight (ffinish) = ".GPLIGCfunctions::time2human($TIME[$task_finish_time_index],'t')."\n";
    $message_out .= "Time of landing = $ltime \n";

    my $deltaheight;
    $deltaheight = $BARO[$minimumheightindex] - $BARO[$maximumheightindex] if ($optimized_for eq "olc-class");
    $message_out .= "Departure: ".$BARO[$minimumheightindex]."m   Finish: ".$BARO[$maximumheightindex]."m   Difference: ${deltaheight}m \n" if ($optimized_for eq "olc-class");

    return ($message_out, $stime, $ltime, $fstart, $ffinish, $deltaheight, $starttime, $finishtime);
}

