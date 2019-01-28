import os
import sys

# Allow importing packages implemented in sub directories
# Prepend as the directory has the same name as the package

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'riotnode'))
