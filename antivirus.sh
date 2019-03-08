#!/bin/bash

display_usage(){
	echo "Usage: $0 [file/director_to_check]"
}

if [ $# -le 0 ]
then
	display_usage
	exit 1
fi

if [ $1 == "--help" ]
then
	display_usage
	exit 1
fi

make &> /dev/null

SPEED=1
DELAY=1
LOSS=0
CORRUPT=0
SCAN_FILE=scan_file

{
	killall link
	killall server
	killall client
} &> /dev/null

./link_emulator/link speed=$SPEED delay=$DELAY loss=$LOSS corrupt=$CORRUPT &> /dev/null &
sleep 1
./server &
sleep 1

./client $1

rm $SCAN_FILE
make clean &> /dev/null
