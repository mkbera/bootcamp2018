#!/bin/bash

for i in {1..100}
do
	./blockhash $1 1 > out.1
	./blockhash $1 32 > out.32
	diff out.1 out.32 > /dev/null
	if [ $? -ne 0 ];
	then
		echo "Run #$i differes"
	else
		echo "Run #$i NO diff"
	fi

done
