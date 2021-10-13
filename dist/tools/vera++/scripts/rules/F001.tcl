#!/usr/bin/tclsh
# Source files should not use the '\r' (CR) character

foreach fileName [getSourceFileNames] {
    if { $fileName == "-" } {
      # can't check the content from stdin
      continue
    }
    set file [open $fileName "r"]
    fconfigure $file -translation lf
    set line [gets $file]
    set lineCounter 1
    while {![eof $file]} {
        set pos [string first "\r" $line]
        if {$pos != -1 && $pos != [expr [string length $line] - 1]} {
            report $fileName $lineCounter "\\r (CR) detected in isolation at position ${pos}"
        }
        set line [gets $file]
        incr lineCounter
    }
    close $file
}
