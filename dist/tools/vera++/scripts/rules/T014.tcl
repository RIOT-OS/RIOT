#!/usr/bin/tclsh
# Source files should refer the Boost Software License

foreach file [getSourceFileNames] {
    set found false
    foreach comment [getTokens $file 1 0 -1 -1 {ccomment cppcomment}] {
        set value [lindex $comment 0]
        if {[string first "Boost Software License" $value] != -1} {
            set found true
            break
        }
    }
    if {$found == false} {
        report $file 1 "no reference to the Boost Software License found"
    }
}
