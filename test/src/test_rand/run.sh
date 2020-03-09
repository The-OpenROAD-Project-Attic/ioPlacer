#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
	exit 2
fi

binary=$1

$binary -no_init < ioPlacer.tcl > log.txt 2>&1
$binary -no_init < ioPlacer2.tcl >> log.txt 2>&1

golden=`cat golden1.def`

pins_start=$(grep -n  "PINS" result1.def | cut -f1 -d: | sed -n 1p)
pins_end=$(grep -n  "PINS" result1.def | cut -f1 -d: | sed -n 2p)
head -n ${pins_end} result1.def | tail -n $((pins_end-pins_start+1)) > new1.def

pins_start=$(grep -n  "PINS" result2.def | cut -f1 -d: | sed -n 1p)
pins_end=$(grep -n  "PINS" result2.def | cut -f1 -d: | sed -n 2p)
head -n ${pins_end} result2.def | tail -n $((pins_end-pins_start+1)) > new2.def

cmp -s "new1.def" "golden1.def" 
diff1=$?

cmp -s "new2.def" "golden2.def" 
diff2=$?

if [ $diff1 = 0 ] && [ $diff2 = 0 ];
then
	exit 0
else
	exit 2
fi
