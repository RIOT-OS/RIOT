"""Test compile_and_test_for_board script."""

import subprocess
import re
import compile_and_test_for_board


def _clean(my_str):
    return re.sub(r'\s+', ' ', my_str)


def test_help_message():
    """Verify that the help message is in the script documentation."""
    script = 'compile_and_test_for_board.py'

    # Read the help message from executing the script
    help_bytes = subprocess.check_output([f'./{script}', '--help'])
    help_msg = help_bytes.decode('utf-8')

    docstring = compile_and_test_for_board.__doc__
    assert _clean(help_msg) in _clean(docstring), ("Help message not in the",
                                                   "documentation")
    help_msg += "garbage"
    assert help_msg not in docstring, ("Negative help message test failed.")
