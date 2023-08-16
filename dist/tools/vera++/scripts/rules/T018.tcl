#!/usr/bin/tclsh
# using namespace are not allowed in header files

foreach fileName [getSourceFileNames] {
    set extension [file extension $fileName]
    if {[lsearch {.h .hh .hpp .hxx .ipp} $extension] != -1} {

        set state "start"
        foreach token [getTokens $fileName 1 0 -1 -1 {using namespace identifier}] {
            set type [lindex $token 3]

            if {$state == "using" && $type == "namespace"} {
                report $fileName $usingLine "using namespace not allowed in header file"
            }

            if {$type == "using"} {
                set usingLine [lindex $token 1]
            }

            set state $type
        }
    }
}
