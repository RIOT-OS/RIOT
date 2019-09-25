from lexer import Token, Lexeme
import sys
import itertools

def handle_comment_potentially_belonging_to_board_list(lexemes, idx, new_lexemes, board_lists):
	same, diff = consume_until_different_token(lexemes, idx, Token.COMMENT)

	if diff.type == Token.BOARD_LIST:
		board_lists.extend(same)
		board_lists.append(diff)
		board_lists.append(Lexeme(Token.EMPTY_LINE, ''))

		# Skip empty line following board list to avoid two
		# subsequent empty lines
		if lexemes[idx + len(same) + 1].type == Token.EMPTY_LINE:
			idx += 1

		return idx + len(same) + 1

	new_lexemes.extend(same)
	return idx + len(same)


def consume_until_different_token(lexemes, idx, token):
	"""Consumes lexemes of type 'token' until a different token type is found."""
	same_type = []
	while lexemes[idx].type == token:
		same_type.append(lexemes[idx])
		idx += 1

	return same_type, lexemes[idx]


def move_board_lists_to_end(lexemes):
	new_lexemes = [] # The re-ordered list of lexemes
	board_lists = []
	final_include = None

	idx = 0
	while lexemes[idx].type != Token.EOF:

		# Comment, potentially belonging to a board list
		if lexemes[idx].type == Token.COMMENT:
			idx = handle_comment_potentially_belonging_to_board_list(lexemes, idx, new_lexemes, board_lists)
			continue

		# Board list without preceding comment
		if lexemes[idx].type == Token.BOARD_LIST:
			board_lists.append(lexemes[idx])
			idx += avoid_subsequent_newlines(lexemes, idx)

		elif lexemes[idx].type == Token.FINAL_INCLUDE:
			final_include = lexemes[idx]
			idx += avoid_subsequent_newlines(lexemes, idx)

		else:
			new_lexemes.append(lexemes[idx])

		idx += 1

	board_lists = format_boardlists(board_lists)
	append_board_lists(new_lexemes, board_lists)

	assure_preceding_newline(new_lexemes)
	if final_include is not None:
		new_lexemes.append(final_include)

	new_lexemes.append(lexemes[idx]) # EOF
	return new_lexemes

def append_board_lists(new_lexemes, board_lists):
	"""Append boards lists, making sure that each is preceded by a newline"""

	assure_newline = True
	for lexeme in board_lists:
		if assure_newline and lexeme.type != Token.EMPTY_LINE:
			assure_preceding_newline(new_lexemes)
			assure_newline = False

		new_lexemes.append(lexeme)
		if lexeme.type == Token.BOARD_LIST:
			assure_newline = True

def avoid_subsequent_newlines(lexemes, idx):
	if lexemes[idx-1].type == Token.EMPTY_LINE and lexemes[idx+1].type == Token.EMPTY_LINE:
		return 1

	return 0


def assure_preceding_newline(lexemes):
	if lexemes[len(lexemes)-1].type != Token.EMPTY_LINE:
		lexemes.append(Lexeme(Token.EMPTY_LINE, ''))


def format_boardlists(board_lists):
	return [format_boardlist(board_list) for board_list in board_lists]

def format_boardlist(board_list):
	if board_list.type != Token.BOARD_LIST:
		return board_list

	operator = board_list.metadata['variable'].match(board_list.content)[1]
	if operator == '+=':
		return board_list

	variable, boards = board_list.content.split(operator)

	variable = variable.strip()
	boards = [x.rstrip('\\').strip().split(' ') for x in boards.split('\n')]
	boards = list(itertools.chain.from_iterable(boards))
	boards = [x.strip() for x in boards if x != '' and x != '#']
	boards.sort()
	boards = " \\\n  ".join(boards)

	content = variable + " := \\\n  " + boards + " \\\n  #"
	return Lexeme(Token.BOARD_LIST, content)


def write(lexemes, target_file):
	lexemes = move_board_lists_to_end(lexemes)
	for lexeme in lexemes:
		target_file.write(lexeme.content)
		if lexeme.type is not Token.EOF:
			target_file.write('\n')

	target_file.flush()
