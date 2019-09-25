#!/usr/bin/env python3
import os
import sys
import subprocess
import re
from pprint import pprint
from tempfile import NamedTemporaryFile
import shutil

import lexer
import writer

def chdir_to_riot_root():
	SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
	RIOT_DIR = os.path.realpath(SCRIPT_DIR + "/../../..")
	os.chdir(RIOT_DIR)

class BoardListVariable:
	def __init__(self, name):
		self.name = name
		self.regex = re.compile('%s\\s*([\\+:]=)(.+)' % (self.name))

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
filenames = list(set(filenames))
filenames.sort()

formatting_errors = False
for filename in filenames:
	source_file = open(filename, 'r', encoding="utf-8")
	temp_file = NamedTemporaryFile(mode='w+', prefix='formatted-makefile-', encoding="utf-8")

	try:
		lexemes = lexer.lex(source_file, TARGET_VARIABLES)
		writer.write(lexemes, temp_file)

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
