#!/bin/sh

pid=`pgrep pseudoterm`

if test "$pid" = "" ; then

    echo "   Pseudoterm not running."

else

    if test "$1" = "continue" ; then
        kill -s USR1 $pid;
    elif test "$1" = "pause" ; then
        kill -s USR2 $pid ;
    elif test "$1" = "stop" ; then
        kill $pid ;
    else
        echo "Usage:";
        echo "termctrl.sh continue/pause/stop";
    fi

fi
