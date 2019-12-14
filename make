#!/bin/bash

if !((gcc -g -c rand.c) && (gcc -g -c dlmall.c)); then
#  echo "SUCCESS"
#else
	echo "ERROR"
	exit 0;
fi;

include_files="rand.o dlmall.o"

case $1 in
	test)
		if gcc -g -o test $include_files test.c -lm; then
			echo "SUCCESS"
		else
			echo "ERROR"
		fi;
		;;
	*)
		if gcc -g -o bench $include_files bench.c -lm; then
			echo "SUCCESS"
		else
			echo "ERROR"
		fi;
		;;
esac

exit 0;
