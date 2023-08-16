#!/usr/bin/tclsh
# Unnamed namespaces are not allowed in header files

foreach fileName [getSourceFileNames] {
    set extension [file extension $fileName]
    if {[lsearch {.h .hh .hpp .hxx .ipp} $extension] != -1} {

        set state "start"
        foreach token [getTokens $fileName 1 0 -1 -1 {namespace identifier leftbrace}] {
            set type [lindex $token 3]

            if {$state == "namespace" && $type == "leftbrace"} {
                report $fileName $namespaceLine "unnamed namespace not allowed in header file"
            }

            if {$type == "namespace"} {
                set namespaceLine [lindex $token 1]
            }

            set state $type
        }
    }
}
