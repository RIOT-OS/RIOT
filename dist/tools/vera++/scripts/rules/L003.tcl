#!/usr/bin/tclsh
# No leading and no trailing empty lines

foreach f [getSourceFileNames] {
    set lineCount [getLineCount $f]
    if {$lineCount > 0} {
        set firstLine [getLine $f 1]
        if {[string trim $firstLine] == ""} {
            report $f 1 "leading empty line(s)"
        }

        set lastLine [getLine $f $lineCount]
        if {[string trim $lastLine] == ""} {
            report $f $lineCount "trailing empty line(s)"
        }
    }
}
