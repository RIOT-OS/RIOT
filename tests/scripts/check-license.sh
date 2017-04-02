#!/bin/bash

export output1=$(combine <(git grep -L -i license | sort | grep '\.c$' -) and <(git grep -L -i lgpl | sort))
output2=$(combine <(echo $output1) and <(git grep -L -i "Permission to use, copy, modify, and/or distribute this software for any" | sort))
echo $output2

if [ "$output" = "" ]; then
	exit 0
else
	exit 1
fi

