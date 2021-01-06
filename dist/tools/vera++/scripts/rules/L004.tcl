#!/usr/bin/tclsh
# Line cannot be too long

set maxLength [getParameter "max-line-length" 100]

#MOD: We add here a line length thershold for line length
set maxLengthWarn [getParameter "max-line-length-warn" 80]

foreach f [getSourceFileNames] {
    set lineNumber 1
    foreach line [getAllLines $f] {
        if {[string length $line] > $maxLength} {
            report $f $lineNumber "line is longer than ${maxLength} characters"
        } elseif {[string length $line] > $maxLengthWarn} {
            # puts won't make vera++ return error code when invoked with --error
            puts "$f:$lineNumber: warning: line is longer than $maxLengthWarn characters"
        }
        incr lineNumber
    }
}
