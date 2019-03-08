#!/bin/bash

display_usage(){
	echo "Usage: $0 [file_to_check]"
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

make

SPEED=1
DELAY=1
LOSS=0
CORRUPT=0

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