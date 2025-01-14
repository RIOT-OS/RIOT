#!/bin/bash

# Path to the examples directory
EXAMPLES_DIR="$(dirname "$0")/../../../examples"

# Get a list of all directories in the examples directory
directories=$(find "$EXAMPLES_DIR" -maxdepth 2 -mindepth 1 -type d -exec basename {} \;)

# Check each directory
missing_entries=()
for dir in $directories; do
    if [ ! -f "$EXAMPLES_DIR/$dir/main.c" ]; then
        continue
    fi

    if [ ! -f "$EXAMPLES_DIR/$dir/README.md" ]; then
        missing_entries+=("$dir")
    fi
done

# Report missing entries
if [ ${#missing_entries[@]} -eq 0 ]; then
    echo "All examples have a README"
    exit 0
else
    echo "The following directories are missing a README:"
    for entry in "${missing_entries[@]}"; do
        echo "- $entry"
    done
    exit 1
fi
