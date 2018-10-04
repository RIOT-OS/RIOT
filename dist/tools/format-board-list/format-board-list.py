#!/usr/bin/env python3
import os
import sys
import subprocess
from pprint import pprint
from tempfile import NamedTemporaryFile
import shutil

def chdir_to_riot_root():
	SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
	RIOT_DIR = os.path.realpath(SCRIPT_DIR + "/../../..")
	os.chdir(RIOT_DIR)

TARGET_VARIABLE = 'BOARD_INSUFFICIENT_MEMORY := '

class BoardList:
	def __init__(self):
		self.boards = []

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
		spacing = len(TARGET_VARIABLE) * ' '
		return TARGET_VARIABLE + (" \\\n" + spacing).join(self.boards)


def get_affected_filenames():
	try:
		output = subprocess.check_output(["git", "grep", TARGET_VARIABLE]).decode("utf-8").strip()
	except subprocess.CalledProcessError as e:
		print("broke:\n", e.output) # TODO

	# git-grep returns a 'filename:occurrence' list of results, one per line
	filenames = [line.split(":", maxsplit=1)[0] for line in output.split("\n")]
	filenames = list(filter(lambda filename: os.path.basename(filename) == 'Makefile', filenames))
	return filenames

def has_more_boards(line):
	return line.rstrip().endswith('\\')

def format_makefile_board_list(source_file, target_file):
	board_list = BoardList()

	# Write to the temp file unmodified until we reach the board list
	while True:
		current_line = source_file.readline()
		if TARGET_VARIABLE in current_line:
			break;
		target_file.write(current_line)

	# Extract the board list
	current_line = current_line[len(TARGET_VARIABLE):]
	board_list.add_boards_from_line(current_line)

	while has_more_boards(current_line):
		current_line = source_file.readline()
		board_list.add_boards_from_line(current_line)

	# Write the board list to formatted file
	target_file.write(str(board_list))
	target_file.write('\n')

	# Write the rest of the file unmodified
	current_line = source_file.readline()
	while current_line is not '':
		target_file.write(current_line)
		current_line = source_file.readline()

	target_file.flush()
	return board_list



if len(sys.argv) != 2 or sys.argv[1] not in ['validate', 'format']:
	print("Usage: format-board-list.py [validate | format]")
	print("  In validation mode, the script will return a non-zero")
	print("  exit code if any file is not correctly formatted. In")
	print("  format mode, it will only return a non-zero exit code if")
	print("  the script fails to perform its job.")
	sys.exit(1)

mode = sys.argv[1]
chdir_to_riot_root()
filenames = get_affected_filenames()
formatting_errors = False
for filename in filenames:
	source_file = open(filename, 'r', encoding="utf-8")
	temp_file = NamedTemporaryFile(mode='w+', prefix='formatted-makefile-', encoding="utf-8")
	format_makefile_board_list(source_file, temp_file)

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
