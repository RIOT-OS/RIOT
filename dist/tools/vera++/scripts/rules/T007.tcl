#!/usr/bin/tclsh
# Semicolons should not be isolated by spaces or comments from the rest of the code

foreach f [getSourceFileNames] {
    foreach t [getTokens $f 1 0 -1 -1 {semicolon}] {
        set line [lindex $t 1]
        set column [lindex $t 2]
        set previousTokens [getTokens $f $line 0 $line $column {}]
        if {$previousTokens == {}} {
            report $f $line "semicolon is isolated from other tokens"
        } else {
            set lastToken [lindex $previousTokens end]
            set lastName [lindex $lastToken 3]
            if {[lsearch {space ccomment} $lastName] != -1} {
                set forTokens [getTokens $f $line 0 $line $column {for leftparen}]
                if {[list [lindex [lindex $forTokens 0] 3] [lindex [lindex $forTokens 1] 3]] != {for leftparen}} {
                    report $f $line "semicolon is isolated from other tokens"
                }
            }
        }
    }
}
