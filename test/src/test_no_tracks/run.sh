#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
	exit 2
fi

binary=$1

$binary -no_init < ioPlacer.tcl > log.txt 2>&1
 
grep -q "missing track structure" log.txt

if [ $? -eq 0 ]; then
    exit 0
else
    exit 2
fi
