#!/bin/bash

test_names=("L1_1"	"L1_2"	"L2_1"	"L2_2"	"RAM_1"	"RAM_2")
test_sizes=(64		128		8192	16384	65536	131072)
INPUT=262144.in
TEST_DIR=test_results

typeset -i size_index test_index NUM_EVENTS run
NUM_EVENTS=2
FLAG=0
size_index=0

rm -rf $TEST_DIR
mkdir $TEST_DIR

size_index=0
while ((size_index < 6)); do

	# create dir for this test
	rm -rf $TEST_DIR/${test_names[$size_index]}
	mkdir $TEST_DIR/${test_names[$size_index]}

	TIME_FILE=$TEST_DIR/${test_names[$size_index]}/times
	ORDERED_TIME_FILE=$TIME_FILE.ordered
	touch $TIME_FILE
	touch $ORDERED_TIME_FILE

	# 3 runs for each test, to accout for 5% max differences in error
	let run=1
	while((run <= 3)); do
		# creates files
		EVENTS_FILE=$TEST_DIR/${test_names[$size_index]}/events.$run
		touch $EVENTS_FILE

		# run all tests
		echo "Running test named" ${test_names[$size_index]} "with n=" ${test_sizes[$size_index]}
		echo -n "	"
		let test_index=0
		while ((test_index < NUM_EVENTS)); do
			echo -n "$test_index "
			./n_body ${test_sizes[$size_index]} $INPUT $test_index >> $EVENTS_FILE
			let test_index++
		done
		echo ""
		
		# average of all execution times saved to times file
		echo -n "$run " >> $TIME_FILE
		awk '{sum+=$3} END {print sum/NR}' $EVENTS_FILE >> $TIME_FILE
		sort -t' ' -k2 $TIME_FILE > $ORDERED_TIME_FILE

		let run++
	done

	# checks 5% error margin
	res=`awk 'BEGIN {res=1} {if (NR == 1) {x=$2} else if (($2 - x) / x > 0.5) {res=0}} END {print res}' $ORDERED_TIME_FILE`

	# if result is true, 5% error margin is satisfied, move on to the next 
	if ((res==1)); then
		let size_index++
	else
		echo "margin error not met, restarting"
	fi

done
