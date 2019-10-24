#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
	exit 2
fi

binary=$1

$binary -c 1 < ioPlacer.tcl > log.txt 2>&1

hpwl=$(grep "Final HPWL" log.txt |  awk '{print $3}')
golden_hpwl=$(grep "Final HPWL" golden.txt |  awk '{print $3}')

upper_limit=$(echo "${golden_hpwl} * 1.05" | bc | cut -f1 -d.)
lower_limit=$(echo "${golden_hpwl} * 0.95" | bc | cut -f1 -d.)

if [ $hpwl == $golden_hpwl ]
then
	exit 0
elif [ $hpwl -gt $lower_limit ] && [ $hpwl -lt $upper_limit ]
then
	exit 1
else
	exit 2
fi
