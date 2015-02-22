#!/bin/bash
set -e

make -s -C ./examples/default info-concurrency
git rebase riot/master || git rebase --abort

./dist/tools/whitespacecheck/check.sh master || exit

./dist/tools/licenses/check.sh master --diff-filter=MR --error-exitcode=0 || exit
./dist/tools/licenses/check.sh master --diff-filter=AC || exit

./dist/tools/doccheck/check.sh master || exit

./dist/tools/externc/check.sh master || exit

# TODO:
#   Remove all but `master` parameters to cppcheck (and remove second
#   invocation) once all warnings of cppcheck have been taken care of
#   in master.
./dist/tools/cppcheck/check.sh master --diff-filter=MR --error-exitcode=0 || exit
./dist/tools/cppcheck/check.sh master --diff-filter=AC || exit

if [[ $BUILDTEST_MCU_GROUP ]]
then
	if [ "$BUILDTEST_MCU_GROUP" == "x86" ]
	then

		make -C ./tests/unittests all test BOARD=native || exit
		# TODO:
		#   Reenable once https://github.com/RIOT-OS/RIOT/issues/2300 is
		#   resolved:
		#   - make -C ./tests/unittests all test BOARD=qemu-i386 || exit
	fi
	./dist/tools/compile_test/compile_test.py
	./dist/tools/pr_check/pr_check.sh riot/master
fi
