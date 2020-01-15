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
	clean)
		rm *.o
		rm bench test
		;;
	timing)
		if gcc -g -o bench_timings $include_files bench_timings.c -lm; then
			echo "SUCCESS"
		else
			echo "ERROR"
		fi;
		;;
	timing_sh)
		if gcc -g -D SMALL_HEADER -o bench_timings_sh $include_files bench_timings.c -lm; then
			echo "SUCCESS"
		else
			echo "ERROR"
		fi;
		;;
	sh)
		if gcc -g -D SMALL_HEADER -o bench $include_files bench.c -lm; then
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
