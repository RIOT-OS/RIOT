#!/bin/bash

output=$(astyle -r *.c *.h)
echo $output

if [ "$output" = "" ]; then
	exit 0
else
	exit 1
fi

