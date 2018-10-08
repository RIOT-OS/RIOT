#!/usr/bin/env python3
import os
import sys
import subprocess
import re
from pprint import pprint
from tempfile import NamedTemporaryFile
import shutil

TARGET_VARIABLES = ['BOARD_INSUFFICIENT_MEMORY', 'BOARD_BLACKLIST', 'BOARD_WHITELIST']

def chdir_to_riot_root():
	SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
	RIOT_DIR = os.path.realpath(SCRIPT_DIR + "/../../..")
	os.chdir(RIOT_DIR)


class BoardList:
	def __init__(self, first_line_of_board_list):
		self.boards = []
		self.target_variable = None
		self.variable_operation = None
		self.regex = re.compile('([A-Z_]+)\\s*([\\+:]?=)(.+)')
		self.parse_first_line(first_line_of_board_list)

	def parse_first_line(self, line):
		m = self.regex.match(line)
		assert m is not None, "Regex should match '%s'" % (line)
		assert m.group(1) in TARGET_VARIABLES, "Match '%s' should be in TARGET_VARIABLES" % (m.group(1))

		self.target_variable = m.group(1)
		self.variable_operation = m.group(2)
		self.add_boards_from_line(m.group(3))

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
		prefix = "%s %s " % (self.target_variable, self.variable_operation)
		spacing = len(prefix) * ' '
		return prefix + (" \\\n" + spacing).join(self.boards)


def get_affected_filenames(target_variable):
	try:
		output = subprocess.check_output(["git", "grep", target_variable]).decode("utf-8").strip()
	except subprocess.CalledProcessError as e:
		print("broke:\n", e.output) # TODO

	# git-grep returns a 'filename:occurrence' list of results, one per line
	filenames = [line.split(":", maxsplit=1)[0] for line in output.split("\n")]
	filenames = list(filter(lambda filename: os.path.basename(filename) == 'Makefile', filenames))
	return filenames


def has_more_boards(line):
	return line.rstrip().endswith('\\')

def consume_until_board_list(source_file, target_file):
	# Write to the temp file unmodified until we reach a board list or EOF
	current_line = source_file.readline()
	while current_line is not '':
		if any(target_variable in current_line for target_variable in TARGET_VARIABLES):
			return BoardList(current_line), current_line

		target_file.write(current_line)
		current_line = source_file.readline()

	return None, current_line

def format_makefile_board_lists(source_file, target_file):

	while True:
		board_list, current_line = consume_until_board_list(source_file, target_file)

		if board_list is None:
			break

		while has_more_boards(current_line):
			current_line = source_file.readline()
			board_list.add_boards_from_line(current_line)

		# Write the board list to formatted file
		target_file.write(str(board_list))
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
