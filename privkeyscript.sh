#!/bin/bash
set -x
file="../data.txt"
while IFS= read -r line
do
        # display $line or do somthing with $line
	 ./src/anon-cli importprivkey "$line"
	 echo "$line"
	 ./src/anon-cli z_gettotalbalance
	 sleep 5m
done <"$file"
