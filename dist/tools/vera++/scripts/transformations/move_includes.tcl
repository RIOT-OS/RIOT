#!/usr/bin/tclsh
# change prefix of #include paths

set oldPrefix "\"boost/"
set newPrefix "\"boom/"

set oldPrefixLength [string length $oldPrefix]

foreach fileName [getSourceFileNames] {
    set tokens [getTokens $fileName 1 0 -1 -1 {}]
    set newFile [open $fileName "w"]
    foreach token $tokens {
        set tokenValue [lindex $token 0]
        set tokenType [lindex $token 3]
        if {$tokenType == "pp_qheader"} {
            set index [string first $oldPrefix $tokenValue]
            if {$index != -1} {
                set tokenValue [string replace $tokenValue $index [expr $index + $oldPrefixLength - 1] $newPrefix]
            }
        }
        puts -nonewline $newFile $tokenValue
    }
    close $newFile
}
