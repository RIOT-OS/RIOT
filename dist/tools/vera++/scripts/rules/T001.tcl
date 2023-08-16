#!/usr/bin/tclsh
# One-line comments should not have forced continuation

foreach f [getSourceFileNames] {
    foreach t [getTokens $f 1 0 -1 -1 {cppcomment}] {
        set lineNumber [lindex $t 1]
        set wholeLine [getLine $f $lineNumber]

        if {[string index $wholeLine end] == "\\"} {
            report $f $lineNumber "line-continuation in one-line comment"
        }
    }
}
