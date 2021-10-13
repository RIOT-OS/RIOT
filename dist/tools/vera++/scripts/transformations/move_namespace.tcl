#!/usr/bin/tclsh
# change the namespace of all source files

set oldNamespace "boost"
set newNamespace "boom"

foreach fileName [getSourceFileNames] {
    set tokens [getTokens $fileName 1 0 -1 -1 {}]
    set newFile [open $fileName "w"]
    foreach token $tokens {
        set tokenValue [lindex $token 0]
        set tokenType [lindex $token 3]
        if {$tokenType == "identifier" && $tokenValue == $oldNamespace} {
            set tokenValue $newNamespace
        }
        puts -nonewline $newFile $tokenValue
    }
    close $newFile
}
