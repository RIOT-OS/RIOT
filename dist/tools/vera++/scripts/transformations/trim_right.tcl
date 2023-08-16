#!/usr/bin/tclsh
# trim each line by removing trailing whitespace

foreach fileName [getSourceFileNames] {
    set lines [getAllLines $fileName]
    set newFile [open $fileName "w"]
    foreach line $lines {
        set newLine [string trimright $line]
        puts $newFile $newLine
    }
    close $newFile
}
