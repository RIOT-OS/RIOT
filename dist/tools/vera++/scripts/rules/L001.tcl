#!/usr/bin/tclsh
# No trailing whitespace

set strictMode [getParameter "strict-trailing-space" 0]

foreach f [getSourceFileNames] {
    set lineNumber 1
    set previousIndent ""
    foreach line [getAllLines $f] {

        if [regexp {^.*[[:space:]]+$} $line] {
            if {$strictMode || [string trim $line] != "" || $line != $previousIndent} {
                report $f $lineNumber "trailing whitespace"
            }
        }

        regexp {^([[:space:]]*).*$} $line dummy previousIndent
        incr lineNumber
    }
}
