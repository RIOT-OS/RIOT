#!/bin/sh

# Split the changelog into releases, and try to convert to restructuredtext
# Author: Juan I Carrano <j.carrano@fu-berlin.de>

# Find section headers, add "break" marker
filter_break() {
    sed -e "/^RIOT-....... -/i\ch-section-break"
}

# put issues into bullets, convert to restructured text, promote heading
md2rst() {
    sed -e "/^#..../s/^/* /" | pandoc -f gfm -t rst | sed -e "/^RIOT-....... -/{
h
s/./=/gp
g
}"
}


get_release() {
    sed -ne "/^RIOT-....... -/{s/^RIOT-\(.......\).\+/\1/;p;q}"
}

split_loop() {
    sed -u Q
    while true ; do
        DOC="$(sed -u "/ch-section-break/Q7")"
        if [ $? == 7 ] ; then
            REL=$(echo "$DOC" | (get_release; cat >/dev/null))
            echo "${DOC}" | md2rst > "${1}/${REL}.rst"
        else
            break
        fi
    done
}

if [ "x${1}" == "x" ] ; then
    echo "Usage: ${0} ouput-dir <release-notes.txt"
    exit
fi

filter_break | split_loop "${1}"
