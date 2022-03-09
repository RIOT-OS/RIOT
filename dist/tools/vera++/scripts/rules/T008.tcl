#!/usr/bin/tclsh
# Keywords catch, for, if and while should be followed by a single space

foreach f [getSourceFileNames] {
    foreach t [getTokens $f 1 0 -1 -1 {catch for if switch while}] {
        set keyword [lindex $t 0]
        set line [lindex $t 1]
        set column [lindex $t 2]
        set followingTokens [getTokens $f $line [expr $column + [string length $keyword]] [expr $line + 1] -1 {}]
        if {[llength $followingTokens] < 2} {
            report $f $line "keyword '${keyword}' not followed by a single space"
        } else {
            if {[list [lindex [lindex $followingTokens 0] 0] [lindex [lindex $followingTokens 1] 0]] != [list " " "("]} {
                report $f $line "keyword '${keyword}' not followed by a single space"
            }
        }
    }
}
