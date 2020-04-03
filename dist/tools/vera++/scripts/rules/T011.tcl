#!/usr/bin/tclsh
# Curly brackets from the same pair should be either in the same line or in the same column

proc acceptPairs {} {
    global file parens index end

    while {$index != $end} {
        set nextToken [lindex $parens $index]
        set tokenValue [lindex $nextToken 0]

        if {$tokenValue == "\{"} {
            incr index
            set leftParenLine [lindex $nextToken 1]
            set leftParenColumn [lindex $nextToken 2]

            acceptPairs

            if {$index == $end} {
                report $file $leftParenLine "opening curly bracket is not closed"
                return
            }

            set nextToken [lindex $parens $index]
            incr index
            set tokenValue [lindex $nextToken 0]
            set rightParenLine [lindex $nextToken 1]
            set rightParenColumn [lindex $nextToken 2]

            if {($leftParenLine != $rightParenLine) && ($leftParenColumn != $rightParenColumn)} {
                # make an exception for line continuation
                set leftLine [getLine $file $leftParenLine]
                set rightLine [getLine $file $rightParenLine]
                if {[string index $leftLine end] != "\\" && [string index $rightLine end] != "\\"} {
                    report $file $rightParenLine "closing curly bracket not in the same line or column"
                }
            }
        } else {
            return
        }
    }
}

foreach file [getSourceFileNames] {
    set parens [getTokens $file 1 0 -1 -1 {leftbrace rightbrace}]
    set index 0
    set end [llength $parens]
    acceptPairs
    if {$index != $end} {
        report $file [lindex [lindex $parens $index] 1] "excessive closing bracket?"
    }
}
