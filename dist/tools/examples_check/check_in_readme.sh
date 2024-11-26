#!/bin/bash

# Path to the examples directory
EXAMPLES_DIR="$(dirname "$0")/../../../examples"

# Path to the README.md file
README_FILE="$EXAMPLES_DIR/README.md"

# Get a list of all directories in the examples directory
directories=$(find "$EXAMPLES_DIR" -maxdepth 1 -mindepth 1 -type d -exec basename {} \;)

# Check each directory
missing_entries=()
for dir in $directories; do
    if ! grep -q "$dir" "$README_FILE"; then
        missing_entries+=("$dir")
    fi
done

# Report missing entries
if [ ${#missing_entries[@]} -eq 0 ]; then
    echo "All directories are listed in the examples/README.md file."
    exit 0
else
    echo "The following directories are missing in the examples/README.md file:"
    for entry in "${missing_entries[@]}"; do
        echo "- $entry"
    done
    exit 1
fi
