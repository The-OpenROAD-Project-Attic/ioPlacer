#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
	exit 2
fi

binary=$1

# Download the testcase
wget -q http://www.ispd.cc/contests/18/ispd18_test2.tgz 
tar -xvzf ispd18_test2.tgz > log.txt

$binary -c < ioPlacer.tcl >> log.txt

golden=`cat golden.def`

pins_start=$(grep -n  "PINS" result.def | cut -f1 -d: | sed -n 1p)
pins_end=$(grep -n  "PINS" result.def | cut -f1 -d: | sed -n 2p)

head -n ${pins_end} result.def | tail -n $((pins_end-pins_start+1)) > new.def

if cmp -s "new.def" "golden.def";
then
	exit 0
else
	exit 2
fi
