#!/bin/bash

if [ -z "$1" ]; then
    FILE=out.top
else
    FILE="$1"
fi

if tput colors &> /dev/null && [ "$(tput colors)" -ge 8 ]; then
    COK="\e[1;32m"
    CWARN="\e[1;33m"
    CRESET="\e[0m"
else
    COK=
    CWARN=
    CRESET=
fi

WIDTH=100
HEIGHT=100
RANGE=30
VARIANCE=15
NUM=10

echo "writing to $FILE"

./bin/topogen -w $WIDTH -h $HEIGHT -r $RANGE -v $VARIANCE -n $NUM > "$FILE"

if ! command -v gnuplot > /dev/null; then
    printf "${CWARN}%s${CRESET}\n" "gnuplot not installed"
    exit 0
fi

printf "${COK}%s${CRESET}\n" "rendering to ${FILE}.svg"
gnuplot -e "set xrange [0:$WIDTH]" 			\
	-e "set yrange [0:$HEIGHT]" 			\
	-e "set terminal svg size $(( WIDTH * 8 )), $(( HEIGHT * 8 ))" \
	-e "set output '$FILE.svg'"			\
	-e "set datafile commentschars '!'"		\
	-e "set style fill transparent solid 0.25"	\
	-e "plot '$FILE' index 1 using 3:4:(\$5*2):(\$5*2):6 with ellipses lc rgb var notitle,	\
	         '$FILE' index 1 using 3:4:2 with labels point pt 2 offset char 0.5,0.5 notitle"
