#!/usr/bin/tclsh
# Line cannot be too long

set maxLength [getParameter "max-line-length" 100]

foreach f [getSourceFileNames] {
    set lineNumber 1
    foreach line [getAllLines $f] {
        if {[string length $line] > $maxLength} {
            report $f $lineNumber "line is longer than ${maxLength} characters"
        }
        incr lineNumber
    }
}
