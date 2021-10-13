#!/usr/bin/tclsh
# Identifiers should not be composed of 'l' and 'O' characters only

foreach file [getSourceFileNames] {
    foreach t [getTokens $file 1 0 -1 -1 {identifier}] {
        set value [lindex $t 0]
        if [regexp {^(l|O)+$} $value] {
            report $file [lindex $t 1] "identifier should not be composed of only 'l' and 'O'"
        }
    }
}
