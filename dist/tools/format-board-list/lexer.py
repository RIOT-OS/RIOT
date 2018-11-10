MAKEFILE_LAST_LINE = 'include $(RIOTBASE)/Makefile.include'

class Token:
	counter = 0

	def __init__(self, name):
		self.name = name
		self.id = Token.counter
		Token.counter += 1

	def __str__(self):
		return self.name

	def __eq__(self, other):
		if not isinstance(other, Token):
			return False

		return self.id == other.id

Token.OTHER = Token('OTHER')
Token.EOF = Token('EOF')
Token.EMPTY_LINE = Token('EMPTY_LINE')
Token.FINAL_INCLUDE = Token('FINAL_INCLUDE')
Token.BOARD_LIST = Token('BOARD_LIST')
Token.COMMENT = Token('COMMENT')


class Lexeme:
	"""A lexeme is an instance of a parsed token with the associated content"""
	def __init__(self, type, content, **kwargs):
		self.type = type 
		self.content = content
		self.metadata = kwargs

	def __str__(self):
		return self.type.name

	def __repr__(self):
		return self.__str__()


def lex(source_file, target_variables):
	lexemes = []
	while True:
		lexeme = scan_next(source_file, target_variables)
		lexemes.append(lexeme)
		if lexeme.type == Token.EOF:
			break

	return lexemes


def scan_next(source_file, target_variables):
	orig_line = source_file.readline()

	if orig_line is '':
		return Lexeme(Token.EOF, '')

	line = orig_line.strip()
	orig_line = orig_line.strip('\n')

	if line == '':
		return Lexeme(Token.EMPTY_LINE, '')

	if line == MAKEFILE_LAST_LINE:
		return Lexeme(Token.FINAL_INCLUDE, line)

	if line.startswith('#'):
		return Lexeme(Token.COMMENT, orig_line)

	for target_variable in target_variables:
		if target_variable.match(line) is not None:
			return scan_board_list(source_file, line, target_variable)

	return Lexeme(Token.OTHER, orig_line)


def has_more_boards(line):
	return line.rstrip().endswith('\\')


def scan_board_list(source_file, first_line, matched_variable):
	content = [first_line]
	current_line = first_line

	while has_more_boards(current_line):
			current_line = source_file.readline().rstrip('\n')
			content.append(current_line)

	return Lexeme(Token.BOARD_LIST, '\n'.join(content), variable=matched_variable)
