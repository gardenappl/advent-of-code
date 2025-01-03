#!/bin/sh

set -o errexit
if [ -z "$1" ]; then
	printf "Usage: ./test.sh dayX.c dayX.txt\n" >&2
	printf "    OR ./test.sh DAYS" >&2
	exit 64
fi

if [ -z "$2" ]; then
	days="$1"
	for day in $(seq 1 "$days"); do
		input="day${day}.txt"
		gcc aoc.c "day${day}.c" -march=native -O2 -Wall -std=c11 -o prog 
		echo "Compiled."
		read -r _
		time ./prog "$input"
		echo "Day ${day} done."
		# read -r _
	done
else
	day="$1"
	input="$2"
	gcc aoc.c "day${day}.c" -march=native -O2 -Wall -std=c11 -o prog 
	echo "Compiled."
	read -r _
	time ./prog "$input"
fi
