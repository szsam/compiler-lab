#!/bin/bash
testcase_dir=pretest

files=$(ls $testcase_dir | grep -E '[0-9]+\.txt' | sort -g)

for testcase in $files
do
	echo Run $testcase
	./build/parser $testcase_dir/$testcase
done
