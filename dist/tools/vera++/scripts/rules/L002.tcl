#!/usr/bin/tclsh
# Don't use tab characters

foreach f [getSourceFileNames] {
    set lineNumber 1
    foreach line [getAllLines $f] {

        if [regexp {\t} $line] {
            report $f $lineNumber "horizontal tab used"
        }

        incr lineNumber
    }
}
