#!/usr/bin/env bash
RIOTBASE=$(git rev-parse --show-toplevel)
CURDIR=$(cd "$(dirname "$0")" && pwd)
UNCRUSTIFY_CFG="$RIOTBASE"/uncrustify-riot.cfg

WHITELIST=$CURDIR/whitelist.txt
BLACKLIST=$CURDIR/blacklist.txt

. "$RIOTBASE"/dist/tools/ci/changed_files.sh
. "$RIOTBASE"/dist/tools/ci/github_annotate.sh

# only consider whitelisted stuff, then filter out blacklist
# note: this also applies changed_files' default filter
FILES=$(changed_files | grep -xf "$WHITELIST" | grep -xvf "$BLACKLIST")

check () {
    for F in $FILES
    do
        uncrustify -c "$UNCRUSTIFY_CFG" -f "$RIOTBASE/$F" \
            --check > /dev/null 2>&1 || {
            echo "file $F needs to be uncrustified."
            echo "Please run 'dist/tools/uncrustify/uncrustify.sh'"

            # Disable error until versioning issue is sorted out.
            #exit 1
            exit 0
        }
    done
    echo "All files are uncrustified!"
}

_annotate_diff() {
    if [ -n "$1" -a -n "$2" -a -n "$3" ]; then
        MSG="Uncrustify proposes the following patch:\n\n$3"
        github_annotate_error "$1" "$2" "${MSG}"
    fi
}

exec_uncrustify () {
    if [ "$(git diff HEAD)" ] ; then
        echo "Please commit all changes before running uncrustify.sh"
        exit 1
    fi
    for F in $FILES
    do
        uncrustify -c "$UNCRUSTIFY_CFG" --no-backup "$RIOTBASE/$F"
    done
    if github_annotate_is_on; then
        DIFF=""
        DIFFFILE=""
        DIFFLINE=""
        git diff HEAD | {
            # see https://stackoverflow.com/a/30064493/11921757 for why we
            # use a sub shell here
            while read line; do
                # parse beginning of new diff
                if echo "$line" | grep -q '^--- .\+$'; then
                    _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
                    DIFF="$line"
                    DIFFFILE=$(echo "$line" |
                        sed 's#^--- \([ab]/\)\?\(.\+\)$#\2#g')
                    DIFFLINE=""
                # we are in a diff currently
                elif [ -n "$DIFF" ]; then
                    # grep first line number of diff including offset to
                    # comment _under_ diff
                    if echo "$line" | \
                       grep -q "@@ -[0-9]\+\(,[0-9]\+\)\? +[0-9]\+\(,[0-9]\+\)\? @@"
                       then
                       # treat hunk as new diff so it is at the corresponding line
                       if [ -n "${DIFFLINE}" ]; then
                           _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
                           DIFF="--- $DIFFFILE\n+++ $DIFFFILE"
                       fi
                       DIFFLINE="$(echo "$line" | sed 's/@@ -\([0-9]\+\).*$/\1/')"
                    fi
                    DIFF="$DIFF\n$line"
                fi
            done
            _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
        }
    else
        echo "$OUT"
    fi
}

if [ "$1" == "--check" ] ; then
    check
else
    github_annotate_setup
    exec_uncrustify
    github_annotate_teardown
fi
