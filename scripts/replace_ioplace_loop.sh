#!/bin/bash

cd ..

ioBin="./IOPlacement/ioPlacement"
replaceBin="./RePlAce/src/RePlAce"

outputdir="outputs"
replaceOutDir="${outputdir}/replace"
ioplaceOutDir="${outputdir}/ioplace"

mkdir -p $replaceOutDir
mkdir -p $ioplaceOutDir

if [[ $# != 1 ]]; then
        echo "you need to give one, and only one, argument"
        echo "the number of iterations"
        exit 1
fi

if [[ ! -f "${ioBin}" ]]; then
        echo "IOPlacement binary not found, you need to compile it before"
        exit 1
fi

if [[ ! -f "${replaceBin}" ]]; then
        echo "RePlAce binary not found, you need to compile it before"
        exit 1
fi

# Main loop:
# for all benchmarks
for i in $(seq 2 10)
do
        echo "Benchmark test${i}"
        curr_test="ISPD18/ispd18_test${i}/ispd18_test${i}"
        replaceExperimentDir="${replaceOutDir}/ispd/ispd18_test${i}.input/experiment"
        # do n iterations (user input)
        for j in $(seq 0 "$1")
        do
                echo "----Iteration $j"
                folder="${ioplaceOutDir}/it${j}/test${i}/logs"
                mkdir -p "$folder"
                replaceExperimentDEF=${j}/ispd18_test${i}.input_final.def

                echo "--------Running RePlAce (for benchmark test ${i})"
                # Run RePlAce to generate a placed DEF
                # Directory ISPD18 must have the files of ISPD18 benchmarks
                if [[ "${j}" == 0 ]]
                then
                        echo "------------Creating input def with test flag"
                        ${replaceBin} \
                                -bmflag ispd \
                                -output ${replaceOutDir}/ \
                                -lef "$curr_test".input.lef \
                                -def "$curr_test".input.def \
                                -fast \
                                > "$folder"/replace_test"${i}"_it"${j}".log
                else
                        ${replaceBin} \
                                -bmflag ispd \
                                -output ${replaceOutDir}/ \
                                -lef "$curr_test".input.lef \
                                -def "$curr_test".input.def \
                                > "$folder"/replace_test"${i}"_it"${j}".log
                fi

                if [ "${j}" -lt 10 ]; then
                        cp "${replaceExperimentDir}00${replaceExperimentDEF}" \
                                "$curr_test".input.def
                elif [ "${j}" -lt 100 ]; then
                        cp "${replaceExperimentDir}0${replaceExperimentDEF}" \
                                "$curr_test".input.def
                else
                        cp "${replaceExperimentDir}${replaceExperimentDEF}" \
                                "$curr_test".input.def
                fi

                echo "--------Running IOPlacement"
                # Run IOPlacement to place the IO pins for the placed DEF
                ioLogDir="${folder}/IOPlacement_test${i}_it${j}.log"
                if [[ "{$i}" == 2 ]]
                then
                        "$ioBin" \
                                -i "${curr_test}.input.def" \
                                -h 3 \
                                -v 4 \
                                -l 1 \
                                -o out.def > "$ioLogDir"
                elif [[ "{$i}" == 3 ]]
                then
                        "$ioBin" \
                                -i "${curr_test}.input.def" \
                                -h 7 \
                                -v 6 \
                                -l 1 \
                                -o out.def > "$ioLogDir"
                elif [[ "{$i}" == 4 ]] || [ "{$i}" == 5 ]
                then
                        "$ioBin" \
                                -i "${curr_test}.input.def" \
                                -h 7 \
                                -v 8 \
                                -l 1 \
                                -o out.def > "$ioLogDir"
                else
                        "$ioBin" \
                                -i "${curr_test}.input.def" \
                                -h 8 \
                                -v 7 \
                                -l 1 \
                                -o out.def > "$ioLogDir"
                fi

                # Moving the output DEF of IOPlacement to the ISPD18
                # directory. With that, this out DEF is the new input DEF in
                # the next iteration
                mv out.def "$curr_test".input.def
        done
done
