#!/usr/bin/tclsh
# Source files should contain the copyright notice

foreach file [getSourceFileNames] {
    set found false
    foreach comment [getTokens $file 1 0 -1 -1 {ccomment cppcomment}] {
        set value [lindex $comment 0]
        if {[string first "copyright" [string tolower $value]] != -1} {
            set found true
            break
        }
    }
    if {$found == false} {
        report $file 1 "no copyright notice found"
    }
}
