#!/bin/sh

echo "running gengetopt / needed after changes in cmdline.ggo"
gengetopt -G <cmdline.ggo

# white space removal
sed -i 's/[ \t]*$//' cmdline.h
sed -i 's/[ \t]*$//' cmdline.c
