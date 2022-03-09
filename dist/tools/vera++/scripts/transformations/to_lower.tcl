#!/usr/bin/tclsh
# transform indentifier names from CamelCase to standard_lower_case

# this list contains exceptional mappings as pairs ?original new?

set exceptions {SOMESpecialName some_special_name SOMEOTHER some_other}

proc transformIdentifier {old} {
    global exceptions

    if [string is lower $old] {
        set new $old
    } else {
        set e [lsearch $exceptions $old]
        if {$e != -1} {
            set new [lindex $exceptions [expr $e + 1]]
        } else {
            set state "upper"
            set new ""
            for {set i 0} {$i != [string length $old]} {incr i} {
                set c [string index $old $i]
                if [string is upper $c] {
                    if {$state == "upper"} {
                        set new ${new}[string tolower $c]
                    } else {
                        set new ${new}_[string tolower $c]
                        set state "upper"
                    }
                } else {
                    set new ${new}${c}
                    set state "lower"
                }
            }
        }
    }

    return $new
}

foreach fileName [getSourceFileNames] {
    set tokens [getTokens $fileName 1 0 -1 -1 {}]
    set newFile [open $fileName "w"]
    foreach t $tokens {
        set tokenValue [lindex $t 0]
        set tokenType [lindex $t 3]
        if {$tokenType == "identifier"} {
            set newToken [transformIdentifier $tokenValue]
        } else {
            set newToken $tokenValue
        }
        puts -nonewline $newFile $newToken
    }
    close $newFile
}
