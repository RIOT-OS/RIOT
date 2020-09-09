#!/usr/bin/tclsh
# control structures should have complete curly-braced block of code

foreach fileName [getSourceFileNames] {

    set state "start"
    set prev ""
    foreach token [getTokens $fileName 1 0 -1 -1 {for if while do leftparen rightparen leftbrace rightbrace semicolon}] {
        set type [lindex $token 3]

        if {$state == "control"} {
            if {$type == "leftparen"} {
                incr parenCount
            } elseif {$type == "rightparen"} {
                incr parenCount -1
                if {$parenCount == 0} {
                    set state "expectedblock"
                }
            }
        } elseif {$state == "expectedblock"} {
            if {$type != "leftbrace"} {
                set line [lindex $token 1]
                report $fileName $line "full block {} expected in the control structure"
            }
            set state "block"
        }

        if {$type == "for" || $type == "if"} {
            set parenCount 0
            set state "control"
        } elseif {$type == "do"} {
            set state "expectedblock"
        } elseif {$type == "while" && $prev != "rightbrace"} {
            set parenCount 0
            set state "control"
        }
        set prev $type
    }
}
