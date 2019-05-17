[ ! -d /RePlAce_out ] && mkdir -p RePlAce_out;

[ ! -d /history ] && mkdir -p history;


# Main loop:
for i in $(seq 2 10); # for all benchmarks
do
	echo "Benchmark test${i}";
	for j in $(seq 0 $1); # do n iterations (user input) 
	do			
		echo "----Iteration $j";

		[ ! -d /history/it${j} ] && mkdir -p history/it${j};
		
		[ ! -d /history/it${j}/test${i} ] && mkdir -p history/it${j}/test${i};	
		
		[ ! -d /history/it${j}/test${i}/logs ] && mkdir -p history/it${j}/test${i}/logs;
		
		echo "--------Running RePlAce (for benchmark test${i})";
		# Run RePlAce to generate a placed DEF
		# Directory ISPD18 must have the files of ISPD18 benchmarks
		if [ $j -eq 0 ];
		then
			echo "------------Creating input def with test flag";
			./RePlAce/src/RePlAce -bmflag ispd -output RePlAce_out/ -lef ISPD18/ispd18_test${i}/ispd18_test${i}.input.lef -def ISPD18/ispd18_test${i}/ispd18_test${i}.input.def -fast > history/it${j}/test${i}/logs/replace_test${i}_it${j}.log;
		else
			./RePlAce/src/RePlAce -bmflag ispd -output RePlAce_out/ -lef ISPD18/ispd18_test${i}/ispd18_test${i}.input.lef -def ISPD18/ispd18_test${i}/ispd18_test${i}.input.def > history/it${j}/test${i}/logs/replace_test${i}_it${j}.log;
		fi

		if [ ${j} -lt 10 ]; then
			cp RePlAce_out/ispd/ispd18_test${i}.input/experiment00${j}/ispd18_test${i}.input_final.def ISPD18/ispd18_test${i}/ispd18_test${i}.input.def;
		elif [ ${j} -lt 100 ]; then
			cp RePlAce_out/ispd/ispd18_test${i}.input/experiment0${j}/ispd18_test${i}.input_final.def ISPD18/ispd18_test${i}/ispd18_test${i}.input.def;
		else
			cp RePlAce_out/ispd/ispd18_test${i}.input/experiment${j}/ispd18_test${i}.input_final.def ISPD18/ispd18_test${i}/ispd18_test${i}.input.def;
		fi
		
		echo "--------Running IOPlacement";
		# Run IOPlacement to place the IO pins for the placed DEF 
		if [ $i -eq 2 ];
		then
			./IOPlacement/ioPlacement -i ISPD18/ispd18_test${i}/ispd18_test${i}.input.def -h 3 -v 4 -l 1 -o out.def > history/it${j}/test${i}/logs/IOPlacement_test${i}_it${j}.log;
		elif [ $i -eq 3 ];
		then
			./IOPlacement/ioPlacement -i ISPD18/ispd18_test${i}/ispd18_test${i}.input.def -h 7 -v 6 -l 1 -o out.def > history/it${j}/test${i}/logs/IOPlacement_test${i}_it${j}.log;
		elif [ $i -eq 4 ] || [ $i -eq 5 ];
		then
			./IOPlacement/ioPlacement -i ISPD18/ispd18_test${i}/ispd18_test${i}.input.def -h 7 -v 8 -l 1 -o out.def > history/it${j}/test${i}/logs/IOPlacement_test${i}_it${j}.log;
		else
			./IOPlacement/ioPlacement -i ISPD18/ispd18_test${i}/ispd18_test${i}.input.def -h 8 -v 7 -l 1 -o out.def > history/it${j}/test${i}/logs/IOPlacement_test${i}_it${j}.log; 
		fi
		
		# Moving the output DEF of IOPlacement to the ISPD18 directory. With that, this out DEF is the new input DEF in the next iteration
		mv out.def ISPD18/ispd18_test${i}/ispd18_test${i}.input.def;	
	done;
done;

	
