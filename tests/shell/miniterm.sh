#!/bin/bash
# Wrapper around miniterm.py to test the shell using a RAW terminal.
# pyterm does not allow testing control characters (ctrl-C, ctrl-D) and also
# messes the input and output, possibly hiding issues with the shell.

die ()
{
    echo "${1}" >&2
    exit 1
}

while getopts ":p:b:" opt; do
  case ${opt} in
    p )
      PORT=$OPTARG
      ;;
    b )
      BAUD=$OPTARG
      ;;
    "?" )
      die "Unrecognized options: $OPTARG"
      ;;
    : )
      die "$OPTARG: Missing argument"
      ;;
  esac
done

if [ -n "${PORT}" -a -n "${BAUD}" ] ; then
    miniterm.py "${PORT}" "${BAUD}"
else
    die "Both -p <port> and -b <baud> are required options"
fi
