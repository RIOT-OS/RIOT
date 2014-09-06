# About

This is a cppcheck wrapper script with appropriate parameters for
checking RIOT.
It accepts a branch name as an argument which is used to limit the
scope of the check. Other parameters will be passed to cppcheck, so
you can further modify its behavior.

# Example usage

Check all files changed in the current branch against the branch named
'master':

    ./dist/tools/cppcheck/check.sh master

Check all files but ignore warnings about unused struct members:

    ./dist/tools/cppcheck/check.sh --suppress=unusedStructMember

Check all files changed in the current branch against the branch named
'master', ignoring warnings about unused struct members:

    ./dist/tools/cppcheck/check.sh master --suppress=unusedStructMember --suppress=unassignedVariable

# What to do about the findings

You should read the code carefully. While cppcheck certainly produces
valuable information, it can also warn about code that is actually OK.
If this happens, you can add an "inline suppression" like this:

    /* cppcheck-suppress passedByValue */
    timex_t timex_add(const timex_t a, const timex_t b);

