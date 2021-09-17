"""Test compile_and_test_for_board script."""

import subprocess

import compile_and_test_for_board


def test_help_message():
    """Verify that the help message is in the script documentation."""
    script = 'compile_and_test_for_board.py'

    # Read the help message from executing the script
    help_bytes = subprocess.check_output([f'./{script}', '--help'])
    help_msg = help_bytes.decode('utf-8')

    docstring = compile_and_test_for_board.__doc__

    assert help_msg in docstring, "Help message not in the documentation"
