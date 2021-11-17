# GPLIGC Installer Windows
# $Id: install_windows.pl 9 2014-08-01 10:52:42Z kruegerh $

use Win32;
use Win32::TieRegistry(Delimiter=>"/");
use File::Copy;

$email ="Hannes.Krueger\@gmail.com";

$close_delay = 60;


print <<ENDE;

GPLIGC and OGIE installation (Windows)
    should work on Win2003, Win2008 and Win7 through Win10, but was not tested on all platforms yet!

(c) 2000-2021 by Hannes Krueger ($email)
read the GPLIGC_Manual.pdf for more information

Enter installation directory. Press enter for default.

ENDE


# Ask for installation directory
installdirinput:
print "[c:\\GPLIGC]:  ";
chomp($installdir=<>);

if ($installdir eq "") {$installdir="c:\\GPLIGC";}

if ($installdir !~ /(^.:\\)/) {print "give path with drive letter\n"; goto installdirinput;}

if ($installdir =~ /[;,!|<>\@\%\&\$\*\?\+]/) {   # ~ are allowed. (8.3 aliases as progra~1) # \s removed, trying whitespaces...
    print "Do not use whitespaces* and/or special characters in the PATH!\n";  #
    goto installdirinput;
}


# check for old installation and configfile
$prev_install = 0;

if (-d $installdir) {
    print "\n$installdir already exists... \n";


    if (-f $installdir."/ogie.ini") {
	print "Configurationfile (ogie.ini) detected... The new default configfile will be installed as ogie-new.ini\n";
	$prev_install=1;
	# move configfile to .old to avoid replacing
	move("$installdir/ogie.ini", "$installdir/ogie-old.ini") || failed_cp();
    }


    if (-f $installdir."/opengligcexplorer.ini") {
	print "Configurationfile (opengligcexplorer.ini) detected... The new default configfile will be installed as ogie-new.ini\n";
	$prev_install=1;
	# move configfile to .old to avoid replacing
	move("$installdir/opengligcexplorer.ini", "$installdir/ogie-old.ini") || failed_cp();
    }

}


# copying files
print "\ncopying files...\n";
my $cmd = "xcopy /f /r /i /e * \"$installdir\" ";
system($cmd) && failed_cp();
print "Finished copying files...\n";


# swap configfiles
if ($prev_install) {
    print "new default config file (ogie.ini) will be installed as ogie-new.ini\n";
    move("$installdir/ogie.ini", "$installdir/ogie-new.ini") || failed_cp();
    move("$installdir/ogie-old.ini", "$installdir/ogie.ini") || failed_cp();
}


#################################################

# setting environment variables
print "\nTrying to set environment variables GPLIGCHOME and PATH\n";

# check for OS
($osname, $edition) = Win32::GetOSName();
print "\nYour operating system is: \"$osname\"  \"$edition\" \n";


@autoexec_OSlist = qw(Win95 Win98 WinME);  # these OS's require ENV setting via autoexec.bat

# these OS's require ENV setting via registry (not sure whether 2008/Win7 have the same structure of the registry... I hope so.
@registry_OSlist = qw(WinNT Win2000 Win2003 WinHomeSrv WinXP WinVista Win2008 Win7 Win8 Win10);

# untested, will try to use registy anyway
@untested_OSlist = qw(Win2003 WinHomeSrv Win2008);

foreach (@autoexec_OSlist) {
    if ($osname =~ /$_/i) {
      print "Sorry, $osname is not longer supported! Maybe it will work anyway..?\n";
      print "GPLIGC/OGIE will be installed anyway.\n";
      autoexec(); }
}

foreach (@registry_OSlist) {
    if ($osname =~ /$_/i) { registry(); }
# ups NT seems to be still ok with cygwin 1.7
#    if ($osname eq "WinNT") {
#      print "Sorry, $osname is not longer supported (by cygwin). Consequently OGIE will not work!\n";
#    }
}

foreach (@untested_OSlist) {
    if ($osname =~ /$_/i) {
      untested();
      registry();
      }
}

print "Sorry, I did not recognize your Windows version...\n";
print "You should write an Email to $email and tell me this: ->$osname<--->$edition<- \n";

failed_env();
install_end();


##############################################


# this adds some settings to AUTOEXEC.BAT
# win95 / 98 / ME
sub autoexec {

    my $path_set = 0;

    if ($ENV{winbootdir}) {

	my $autoexec = substr($ENV{winbootdir},0,2) .'\autoexec.bat';

	# make it writable if necessary
	if (-e $autoexec && ! -w $autoexec) {
    	    chmod 0755, $autoexec;
	}


	my @autoexec_bat;

	# read autoexec.bat into @autoexec_bat
	if (open(AUTOR, "<$autoexec")) {
		     @autoexec_bat = <AUTOR>;
	} else { warn "Cannot read autoexec.bat\n"; failed_env(); install_end(); }


	# alter autoexec_bat in mem
	$alter = 0;
	$home_line = 0;
	for ($i=0; $i<=$#autoexec_bat; $i++) {

	    	if ( $autoexec_bat[$i] =~ /GPLIGCHOME/ ) {
		    print "Found this line in autoexec.bat: $autoexec_bat[$i]\n";
		    $autoexec_bat[$i] = "SET GPLIGCHOME=$installdir\n";
		    $alter = 0.5; # half is done
		    $home_line = $i;
		    print "Changed to: $autoexec_bat[$i]\n";

		    # prev line is probably the PATH line added by old installscript
		    if ($autoexec_bat[$i-1] =~ /PATH/ ) {
			print "Found this line in autoexec.bat: $autoexec_bat[$i-1]\n";
			$autoexec_bat[$i-1] = "SET PATH=$installdir;%PATH%\n";
			$alter = 1; # everything is done
			print "Changed to: $autoexec_bat[$i-1]\n";
		    }

		}
	}


	# hmmm, maybe we set the path a second time now...
	# (if previous install wasnt made by the install-script!)
	if ($alter == 0.5) {
	    splice(@autoexec_bat,$home_line,0, "SET PATH=$installdir;%PATH%\n");
	    print "The line SET PATH=$installdir;%PATH% was inserted before the line containing SET GPLIGCHOME\n";


	}

	if ($alter == 0) {
	    push(@autoexec_bat, "SET PATH=$installdir;%PATH%\n");
	    push(@autoexec_bat, "SET GPLIGCHOME=$installdir\n");
	    print "Environment varibles are set!\n";
	}


	# write back autoexec.bat
	if (open(AUTOW, ">$autoexec")) {

	    print "writing back $autoexec ... \n";

	    # write autoexec_bat
	    foreach (@autoexec_bat) {
		print AUTOW "$_";
	    }

	    close AUTOW;
            $path_set=1;

	} else {
    	    warn "Unable to open $autoexec for writing: $!\n";
	    failed_env();
	    install_end();
        }

    } else {
	warn "No winbootdir environment variable found.\n";
	failed_env();
	install_end();
    }


    unless ($path_set) {
        failed_env();
	install_end();
    }

    print "\nInstallation finished\nReboot to make the new settings in AUTOEXEC.BAT take effect\n";
    install_end();

}


# this sets the path in the registry
sub registry {

## I'm not sure if on every (2000/NT4/XP) platform the environment is
## in the same place in registry....

    print "Modifying registry...\n";


    #$environment = $Registry->{"LMachine/System/CurrentControlSet/Control/Session Manager/Environment/"};
    $environment = $Registry->{"CUser/Environment/"};

    my $path = $environment->{"/Path"};
    #print "Path : $path \n";


    # nur wenn nicht schon drinnen /\: in $installdir????
    $quotet_installdir=quotemeta($installdir);

    if ($path !~ /$quotet_installdir/) {
	# we have to put the new path before the rest (maybe there is an old installation?!
	$path = $installdir.";".$path;
	print "$installdir added to Path \n";
    } else { print "Path already set...\n"; }


    #setting environment variables in Registry

    $environment->{"/Path"} = $path;
    $environment->{"/GPLIGCHOME"} = $installdir;

    $path_new = $environment->{"/Path"};
    $installdir_new = $environment->{"/GPLIGCHOME"};

    # check the reread values
    my $failed = 0;
    if ($path eq $path_new) {
	print "\nPath is updated in HKEY_CURRENT_USER/Environment\n";
    } else {
	print "\nSetting Path in HKEY_CURRENT_USER/Environment _failed_ !!!\n";
	$failed = 1;

    }

    if ($installdir eq $installdir_new) {
	print "GPLIGCHOME is set in HKEY_CURRENT_USER/Environment\n";
    } else {
	print "Setting GPLIGCHOME in HKEY_CURRENT_USER/Environment _failed_ !!!\n";
	$failed = 1;
    }

    if ($failed == 0) {
        print "\nEnvironment Variables are set for current user!\n";
	print "Log out and in to make changes in Registry take effect\n";
	print "\nInstallation finished\n";
    } else {
	failed_env();
    }

    install_end();
}


##############################################


sub install_end {
    print "Window will be closed in $close_delay seconds.\n";
    sleep $close_delay;
    exit ;
}

sub failed_env {
    print "\Setting the environment variables failed.\n";
    print "You need to set the enviroment variable GPLIGCHOME to $installdir\n";
    print "and add $installdir to PATH by yourself.\n\n";
    print "Hhow to do this is described in the GPLIGC-manual.\n";
    $close_delay = 240;
}

sub failed_cp {
    print "\nInstallation failed!";
    print "Failure while copying or moving files... Maybe you do not have write permissions for $installdir ?!\n";
    print "If you cannot solve this feel free to contact me $email\n";
    $close_delay = 240;
    install_end();
}

sub untested {
    print "\nGPLIGC/OGIE have not yet been tested on your Windows platform!\n";
    print "Please check whether the installation was succesfull and write me a short notice\n";
    print ">>> $email \n";
    print "provide this line with your report: >>>$osname<<< >>>$edition<<< \n";
    print "Thank you very much!\n";
    #$close_delay = 240;
}
