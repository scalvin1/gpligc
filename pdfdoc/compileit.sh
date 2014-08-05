#!/bin/sh


echo "creating gif and small jpeg"
cd jpg
 convert logo.jpg -sample 400 -colors 16 logos.gif
 gifsicle --batch -O2 logos.gif
 convert logo.jpg -quality 95 -sample 400 logos.jpg

cd ..

#svnversion -n .. >svnversion-number

echo "building pdf manual using pdflatex"
pdflatex GPLIGC_Manual.tex
bibtex GPLIGC_Manual
pdflatex GPLIGC_Manual.tex
pdflatex GPLIGC_Manual.tex


cp -v jpg/logos.jpg ../images/
cp -v jpg/logos.gif ../images/
cp -v jpg/logo.jpg  ../images/

cp -v GPLIGC_Manual.pdf ../doc/
