#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
	exit 2
fi

binary=$1

$binary -no_init < ioPlacer.tcl > log.txt 2>&1

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
