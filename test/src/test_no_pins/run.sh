#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
	exit 2
fi

binary=$1

$binary -no_init < ioPlacer.tcl > log.txt 2>&1

grep "Design without pins. Exiting..." log.txt >> grep_log.txt

if [ $? -eq 0 ]; then
    exit 0
else
    exit 2
fi
