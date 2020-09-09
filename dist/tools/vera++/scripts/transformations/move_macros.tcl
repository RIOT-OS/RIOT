#!/usr/bin/tclsh
# change the prefix in all macros

set oldPrefix "BOOST"
set newPrefix "BOOM"

set oldPrefixLength [string length $oldPrefix]

foreach fileName [getSourceFileNames] {
    set tokens [getTokens $fileName 1 0 -1 -1 {}]
    set newFile [open $fileName "w"]
    foreach token $tokens {
        set tokenValue [lindex $token 0]
        set tokenType [lindex $token 3]
        if {$tokenType == "identifier" && \
                [string compare -length $oldPrefixLength $oldPrefix $tokenValue] == 0} {
            set tokenValue [string replace $tokenValue 0 [expr $oldPrefixLength - 1] $newPrefix]
        }
        puts -nonewline $newFile $tokenValue
    }
    close $newFile
}
