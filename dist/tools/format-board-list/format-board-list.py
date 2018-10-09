#!/usr/bin/env python3
import os
import sys
import subprocess
import re
from pprint import pprint
from tempfile import NamedTemporaryFile
import shutil


MAKEFILE_LAST_LINE = 'include $(RIOTBASE)/Makefile.include'

def chdir_to_riot_root():
	SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
	RIOT_DIR = os.path.realpath(SCRIPT_DIR + "/../../..")
	os.chdir(RIOT_DIR)

class BoardListVariable:
	def __init__(self, name):
		self.name = name
		self.regex = re.compile('%s\\s*([\\+:]?=)(.+)' % (self.name))

	def match(self, line):
		return self.regex.match(line)

TARGET_VARIABLES = [
	BoardListVariable('BOARD_INSUFFICIENT_MEMORY'),
	BoardListVariable('BOARD_BLACKLIST'),
	BoardListVariable('BOARD_WHITELIST')
]

class BoardList:
	def __init__(self, matched_variable, first_line_of_board_list):
		self.boards = []
		self.matched_variable = matched_variable
		self.variable_operation = None
		self.parse_first_line(first_line_of_board_list)

	def parse_first_line(self, line):
		m = self.matched_variable.match(line)
		assert m is not None, "Regex should match '%s'" % (line)

		self.variable_operation = m.group(1)
		self.add_boards_from_line(m.group(2))

	def add_boards_from_line(self, line):
		boards = self._extract_boards(line)
		self.boards.extend(boards)

	def _extract_boards(self, line):
		cleaned_line = line.strip().rstrip('\\').rstrip()
		boards = cleaned_line.split(' ')
		boards = list(map(lambda board: board.strip(), boards))
		boards = list(filter(lambda board: len(board) > 0, boards))
		return boards

	def __str__(self):
		self.boards.sort()
		prefix = "%s %s " % (self.matched_variable.name, self.variable_operation)
		spacing = len(prefix) * ' '
		return prefix + (" \\\n" + spacing).join(self.boards)


def get_affected_filenames(target_variable):
	try:
		output = subprocess.check_output(["git", "grep", target_variable.name]).decode("utf-8").strip()
	except subprocess.CalledProcessError as e:
		print("broke:\n", e.output) # TODO

	# git-grep returns a 'filename:occurrence' list of results, one per line
	filenames = [line.split(":", maxsplit=1)[0] for line in output.split("\n")]
	filenames = list(filter(lambda filename: os.path.basename(filename) == 'Makefile', filenames))
	return filenames


def has_more_boards(line):
	return line.rstrip().endswith('\\')

def consume_until_board_list(source_file, target_file):
	current_line = 'x'
	previous_line = ''

	# When we're moving things around within the Makefile, such as a board list
	# or the base Makefile include, we need to skip newlines afterwards, since
	# otherwise we'd have several newlines in a row, e.g.
	#
	# newline
	# line that is moved to the end of the file
	# newline
	#
	skip_newlines = True

	# Write to the target_file unmodified until we reach a board list or EOF
	while current_line is not '':
		previous_line = current_line
		current_line = source_file.readline()

		if skip_newlines and current_line.strip() is '':
			continue

		skip_newlines = False

		if current_line.strip() == MAKEFILE_LAST_LINE:
			current_line = '\n'
			skip_newlines = True
			continue

		for target_variable in TARGET_VARIABLES:
			if target_variable.match(current_line) is not None:
				return BoardList(target_variable, current_line), current_line, previous_line

		target_file.write(current_line)

	return None, current_line, previous_line

def format_makefile_board_lists(source_file, target_file):
	board_lists = []

	previous_line = ''
	while True:
		board_list, current_line, previous_line = consume_until_board_list(source_file, target_file)

		# If consume_until_board_list returns None we've reached the end of the file
		if board_list is None:
			break

		while has_more_boards(current_line):
			current_line = source_file.readline()
			board_list.add_boards_from_line(current_line)

		board_lists.append(board_list)
		previous_line = current_line

	if previous_line.strip() != '':
		target_file.write('\n')

	# Write the board list to formatted file
	for board_list in board_lists:
		target_file.write(str(board_list))
		target_file.write('\n\n')

	target_file.write(MAKEFILE_LAST_LINE)
	target_file.write('\n')
	target_file.flush()


if len(sys.argv) != 2 or sys.argv[1] not in ['validate', 'format']:
	print("Usage: format-board-list.py [validate | format]")
	print("  In validation mode, the script will return a non-zero")
	print("  exit code if any file is not correctly formatted. In")
	print("  format mode, it will only return a non-zero exit code if")
	print("  the script fails to perform its job.")
	sys.exit(1)

mode = sys.argv[1]
chdir_to_riot_root()

filenames = []
for target_variable in TARGET_VARIABLES:
	filenames.extend(get_affected_filenames(target_variable))
filenames = set(filenames)

formatting_errors = False
for filename in filenames:
	source_file = open(filename, 'r', encoding="utf-8")
	temp_file = NamedTemporaryFile(mode='w+', prefix='formatted-makefile-', encoding="utf-8")

	try:
		format_makefile_board_lists(source_file, temp_file)
	except AssertionError as e:
		print("Error in file %s: %s" % (source_file, e))
		sys.exit(1)

	if mode == 'validate':
		diff = subprocess.Popen(['diff', '-u', filename, temp_file.name], stdout=subprocess.PIPE)
		return_code = diff.wait()
		if return_code != 0:
			formatting_errors = True
			print(diff.stdout.read().decode(encoding='utf-8'))
	elif mode == 'format':
		shutil.copyfile(temp_file.name, source_file.name)

if formatting_errors:
	sys.exit(1)
