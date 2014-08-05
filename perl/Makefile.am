bin_SCRIPTS = gpligc
dist_bin_SCRIPTS = gpsp2igcfile.pl gpsp2igc.pl loopviewer.pl

libgpligcdir = $(libdir)/gpligc
dist_libgpligc_SCRIPTS = GPLIGCfunctions.pm

CLEANFILES = gpligc
EXTRA_DIST = GPLIGC.pl

do_subst = sed -e 's,zzLIBDIRzz,$(libdir)/gpligc,g' \
	    -e 's,zzDATADIRzz,$(datadir)/gpligc,g'


gpligc: GPLIGC.pl Makefile
	$(do_subst) < $(srcdir)/GPLIGC.pl > gpligc
	chmod +x gpligc
