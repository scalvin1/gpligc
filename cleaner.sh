echo "this cleans all files created by autotools..."
echo "autoreconf --install   to re-create"

rm -r autom4te.cache 
rm Makefile.in aclocal.m4 config.h.in configure
rm depcomp install-sh missing
rm gpligc-?.??*.tar.gz
rm perl/fit.log

rm configure.scan
rm autoscan-*.log
rm config.h.in~

rm src/Makefile.in
rm perl/Makefile.in
rm images/Makefile.in

rm config.sub config.guess

# remove files created by ../make prep
rm images/*.jpg images/*.gif images/*.xbm images/*.png
rm doc/GPLIGC_Manual.pdf

echo "clean pdfdoc"
cd pdfdoc; sh cleaner.sh
