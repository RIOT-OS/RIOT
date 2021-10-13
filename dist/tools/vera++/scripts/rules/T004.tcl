#!/usr/bin/tclsh
# Some keywords should be immediately followed by a colon

set keywords {
    default
    private
    protected
    public
}

proc isKeyword {s} {
    global keywords
    return [expr [lsearch $keywords $s] != -1]
}

foreach f [getSourceFileNames] {
    set lastKeywordLine 0
    set lastKeywordColumn 0
    set lastKeywordValue ""
    set last ""
    foreach t [getTokens $f 1 0 -1 -1 [concat $keywords colon]] {
        set tokenValue [lindex $t 0]
        set tokenName [lindex $t 3]
        if {$tokenName == "colon"} {
            if {$last == "keyword" && $lastKeywordLine != 0} {
                set line [lindex $t 1]
                set column [lindex $t 2]
                if {$line != $lastKeywordLine ||
                    $column != [expr $lastKeywordColumn + [string length $lastKeywordValue]]} {
                    set nonWhiteFound "false"
                    foreach tb [getTokens $f $lastKeywordLine [expr $lastKeywordColumn + 1] $line $column {}] {
                        set tbName [lindex $tb 3]
                        if {[lsearch {space newline ccomment cppcomment} $tbName] == -1} {
                            set nonWhiteFound "true"
                            break
                        }
                    }
                    if {$nonWhiteFound == "false"} {
                        report $f $line "colon not immediately after the \'$lastKeywordValue\' keyword"
                    }
                }
            }
            set last "colon"
        } else {
            set lastKeywordLine [lindex $t 1]
            set lastKeywordColumn [lindex $t 2]
            set lastKeywordValue $tokenValue
            set last "keyword"
        }
    }
}
