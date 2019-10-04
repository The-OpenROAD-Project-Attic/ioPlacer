#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
	exit 2
fi

binary=$1

# Download the testcase
wget -q http://www.ispd.cc/contests/18/ispd18_test1.tgz 
tar -xvzf ispd18_test1.tgz > log.txt

$binary -c 1 < ioPlacer.tcl >> log.txt

grep "> Design without pins. IOPlacer will exit..." log.txt >> grep_log.txt

if [ $? -eq 0 ]; then
    exit 0
else
    exit 2
fi
