#!/usr/bin/tclsh
# Calls to min/max should be protected against accidental macro substitution

foreach file [getSourceFileNames] {
    foreach identifier [getTokens $file 1 0 -1 -1 {identifier}] {
        set value [lindex $identifier 0]
        if {$value == "min" || $value == "max"} {
            set lineNumber [lindex $identifier 1]
            set columnNumber [expr [lindex $identifier 2] + [string length $value]]
            set restOfLine [string range [getLine $file $lineNumber] $columnNumber end]

            if {[regexp {^[[:space:]]*\(} $restOfLine] == 1} {
                report $file $lineNumber "min/max potential macro substitution problem"
            }
        }
    }
}
