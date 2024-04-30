#!/bin/bash

handle_urc=0
echo=0
rcv_eol_1=""
rcv_eol_2=""
send_eol=""

run_test() {
	echo "================================="
	echo "Running test with:"
	echo "  URC handling = $handle_urc"
	echo "  echo         = $echo"
	echo "  send EOL     = $send_eol"
	echo "  rcv EOL 1    = $rcv_eol_1"
	echo "  rcv EOL 2    = $rcv_eol_2"

	make -j --silent BOARD=native "HANDLE_URC=$handle_urc" "ECHO_ON=$echo" "SEND_EOL=\"$send_eol\"" "RECV_EOL_1=\"$rcv_eol_1\"" "RECV_EOL_2=\"$rcv_eol_2\""

	# take /dev/ttyS0 as serial interface. It is only required s.t. UART
	# initialization succeeds and it gets turned off right away.
	set +e
	if ! ./bin/native/tests_at_unit.elf -c /dev/ttyS0 <<< "s\n";
	then
		echo "================================================================================"
		echo "Test failed! Generating compile-commands.json of the last build configuration..."
		echo "================================================================================"
		make -j --silent BOARD=native "HANDLE_URC=$handle_urc" "ECHO_ON=$echo" "SEND_EOL=\"$send_eol\"" "RECV_EOL_1=\"$rcv_eol_1\"" "RECV_EOL_2=\"$rcv_eol_2\"" compile-commands
		exit 1
	fi
	set -e
}

# set -x
set -e

for urc_i in 0 1; do
	handle_urc=$urc_i
	for echo_i in 0 1; do
		echo=$echo_i
		# 0xd == \r, 0xa == \n - I'm using this notation because I can't wrap my head around
		# how many parsers along the way try to interpret \r and \n. Worse, every time the
		# string is entering a new parser, one `\` is shaved off. So it looks like four
		# times is the right amount so that the final C string will be something like "\xd".
		for send_i in "\\\\xd" "\\\\xa" "\\\\xd\\\\xa"; do
			send_eol=$send_i

			rcv_eol_1="\\\\xd"
			rcv_eol_2="\\\\xa"
			run_test

			rcv_eol_1="\\\\xd"
			rcv_eol_2=""
			run_test

			rcv_eol_1="\\\\xa"
			rcv_eol_2=""
			run_test
		done

	done
done

echo "================="
echo "All tests passed!"
echo "================="
