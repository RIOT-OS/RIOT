#!/usr/bin/tclsh
# Keywords return and throw should be immediately followed by a semicolon or a single space

foreach f [getSourceFileNames] {
    foreach t [getTokens $f 1 0 -1 -1 {return throw}] {
        set keyword [lindex $t 0]
        set line [lindex $t 1]
        set column [lindex $t 2]
        set followingTokens [getTokens $f $line [expr $column + [string length $keyword]] [expr $line + 1] 0 {}]
        if {$followingTokens == {}} {
            report $f $line "keyword '${keyword}' not immediately followed by a semicolon or a single space"
        } else {
            set first [lindex [lindex $followingTokens 0] 0]
            if {$first != ";" && $first != " "} {
                if {!([llength $followingTokens] >= 2 && $keyword == "throw" && $first == "(" && [lindex [lindex $followingTokens 1] 0] == ")")} {
                    report $f $line "keyword '${keyword}' not immediately followed by a semicolon or a single space"
                }
            }
        }
    }
}
