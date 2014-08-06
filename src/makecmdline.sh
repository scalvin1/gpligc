#!/bin/sh

echo "running gengetopt / needed after changes in cmdline.ggo"
gengetopt -G <cmdline.ggo
