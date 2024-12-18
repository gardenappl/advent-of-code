#!/bin/sh
if [ -z "$2" ]; then
	printf "Usage: ./test.sh dayX.c dayX.txt\n" >&2
	exit 64
fi
gcc aoc.c "$1" -march=native -O2 -Wall -std=c11 -o prog && time ./prog "$2"
