#!/bin/sh
if [ -z "$2" ]; then
	printf "Usage: ./test.sh dayX.c dayX.txt\n" >&2
	exit 64
fi
gcc aoc.c "$1" -Wall -o prog && ./prog "$2"
