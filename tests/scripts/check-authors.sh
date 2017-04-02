#!/bin/bash

output=$(git grep -L -i -E 'author|PJRC.COM' |grep '\.c$' -)
echo $output

if [ "$output" = "" ]; then
	exit 0
else
	exit 1
fi

