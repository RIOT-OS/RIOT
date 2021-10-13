#!/usr/bin/tclsh
# There should not be too many consecutive empty lines

set maxEmptyLines [getParameter "max-consecutive-empty-lines" 2]

foreach f [getSourceFileNames] {
    set lineNumber 1
    set emptyCount 0
    set reported false
    foreach line [getAllLines $f] {
        if {[string trim $line] == ""} {
            incr emptyCount
            if {$emptyCount > $maxEmptyLines && $reported == "false"} {
                report $f $lineNumber "too many consecutive empty lines"
                set reported true
            }
        } else {
            set emptyCount 0
            set reported false
        }
        incr lineNumber
    }
}
