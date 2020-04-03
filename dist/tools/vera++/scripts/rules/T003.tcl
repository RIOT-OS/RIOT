#!/usr/bin/tclsh
# Some keywords should be followed by a single space

set keywords {
    case
    class
    delete
    enum
    explicit
    extern
    goto
    new
    struct
    union
    using
}

proc isKeyword {s} {
    global keywords
    return [expr [lsearch $keywords $s] != -1]
}

set state "other"
foreach f [getSourceFileNames] {
    foreach t [getTokens $f 1 0 -1 -1 {}] {
        set tokenValue [lindex $t 0]
        set tokenName [lindex $t 3]
        if {$state == "keyword"} {
            if {$tokenName == "space" && $tokenValue == " "} {
                set state "space"
            } else {
                report $f $lineNumber "keyword \'${keywordValue}\' not followed by a single space"
                set state "other"
            }
        } elseif {$state == "space"} {
            if {$tokenName == "newline"} {
                report $f $lineNumber "keyword \'${keywordValue}\' not followed by a single space"
            }
            set state "other"
        } else {
            if [isKeyword $tokenName] {
                set state "keyword"
                set lineNumber [lindex $t 1]
                set keywordValue [lindex $t 0]
            }
        }
    }
}
