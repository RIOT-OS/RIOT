#!/usr/bin/tclsh
foreach f [getSourceFileNames] {
    puts "Tokens in file ${f}:"
    foreach t [getTokens $f 1 0 -1 -1 {}] {
        set value [lindex $t 0]
        set line [lindex $t 1]
        set column [lindex $t 2]
        set name [lindex $t 3]

        puts "${line}/${column}\t${name}\t${value}"
    }
    puts ""
}
