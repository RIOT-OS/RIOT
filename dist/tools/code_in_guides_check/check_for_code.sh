#!/bin/bash
# This script checks if the code blocks in markdown files under the guides directory
# are present in the corresponding source files. It looks for code blocks marked with
# ```c and checks if the code exists in the source files defined by the code_folder
# variable in the markdown file. If the code block is not found, an error is
# reported via GitHub annotations (when run in a GitHub Action) or printed to
# stdout, and the script exits with a non-zero status.

# Define the guides directory
BASE_DIR="$(git rev-parse --show-toplevel)"
GUIDES_DIR="$BASE_DIR/doc/guides"

# shellcheck source=dist/tools/ci/github_annotate.sh
. "$BASE_DIR"/dist/tools/ci/github_annotate.sh

github_annotate_setup

# Check if directory exists
if [ ! -d "$GUIDES_DIR" ]; then
    if github_annotate_is_on; then
        github_annotate_error_no_file "Guides directory not found at $GUIDES_DIR"
    else
        echo "Error: Guides directory not found at $GUIDES_DIR"
    fi
    github_annotate_teardown
    exit 1
fi

exit_code=0

# Reports an error for a file at a given line
# Args: $1 -> Line number of the error
#       $2 -> The file path
#       $3 -> The error message
report_error() {
    line_number=$1
    file_path=$2
    message=$3
    rel_file_path="${file_path#"$BASE_DIR"/}"
    if github_annotate_is_on; then
        github_annotate_error "$rel_file_path" "$line_number" "😥 $message"
    else
        echo "$rel_file_path:$line_number: $message"
    fi
}

# Parses code blocks from within a file
# Args: $1 -> Line number of the code block
#       $2 -> The file path
# Returns:
#       0 -> On found or skipped block
#       1 -> Not found
parse_code_block() {
    start_line=$1
    file_path=$2
    # Check whether there is a <!--skip ci--> comment in the line before the code block
    skip_ci=$(sed -n "$((start_line - 1))p" "$file_path" | grep -c '<!--skip ci-->')
    if [ "$skip_ci" -gt 0 ]; then
        return 0
    fi

    # Extract the code block content until the ending ```
    code_block=$(awk -v start="$start_line" 'NR>start{if(/^```$/){exit}; print}' "$file_path")

    # Check if this code exists in any file in the source directory
    found=0
    # Exclude `/bin/` folders
    INPUT_FILES=$(
        find "$SOURCE_DIR" \
        -type d -name "bin" -prune\
        -or -type f -print\
    )
    for src_file in $INPUT_FILES;  do
        # Read the file content
        src_content=$(cat "$src_file")

        # Check if the code block exists in the current source file
        if [[ "$src_content" == *"$code_block"* ]]; then
            found=1
            break
        fi
    done

    if [ $found -eq 0 ]; then
        report_error "$start_line" "$file_path" \
            "Code block not found in any source file of $code_folder 📚"
        return 1
    fi
}

markdown_files=$(find "$GUIDES_DIR" -type f \( -name "*.md" -o -name "*.mdx" \))

# Find and process all .md and .mdx files
for file in $markdown_files; do
    # Check if there is a code_folder defined in the markdown file
    code_folder=$(grep -oP -m1 'code_folder:\s*\K.+' "$file")
    if [ -z "$code_folder" ]; then
        continue
    fi
    SOURCE_DIR="$BASE_DIR/$code_folder"

    # Get all code block start lines
    code_block_starts=$(grep -n '```[cC]' "$file" | cut -d':' -f1)

    if [ -z "$code_block_starts" ]; then
        report_error 1 "$file" \
            "No code blocks found even though code_folder was specified 📃"
        exit_code=1
    fi

    # Process each code block
    for start_line in $code_block_starts; do
        parse_code_block "$start_line" "$file"
        if [ "$?" -eq "1" ]; then
            exit_code=1
        fi
    done
done

github_annotate_teardown

exit $exit_code
