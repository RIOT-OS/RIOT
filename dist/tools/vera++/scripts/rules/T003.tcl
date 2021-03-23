#!/usr/bin/tclsh
# Some keywords should be followed by a single space
# MOD: This script has been modified to ignore C++ keywords in C files.

set c_keywords {
    case
    enum
    extern
    goto
    struct
    union
}

set cpp_keywords {
    class
    delete
    explicit
    new
    using
}

proc isCKeyword {s} {
    global c_keywords
    return [expr [lsearch $c_keywords $s] != -1]
}

proc isCPPKeyword {s} {
    global cpp_keywords
    return [expr [lsearch $cpp_keywords $s] != -1]
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
            # Check if it's a C++ file
            regexp {\.[ch]pp} $f m
            # C keywords are valid in C++ but not the other way around
            if {[isCKeyword $tokenName] || ([info exists m] && [isCPPKeyword $tokenName])} {
                set state "keyword"
                set lineNumber [lindex $t 1]
                set keywordValue [lindex $t 0]
            }
        }
    }
}
