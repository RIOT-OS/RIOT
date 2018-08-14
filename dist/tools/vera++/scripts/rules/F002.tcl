#!/usr/bin/tclsh
# File names should be well-formed

set maxDirectoryDepth [getParameter "max-directory-depth" 8]
set maxDirnameLength [getParameter "max-dirname-length" 31]
set maxFilenameLength [getParameter "max-filename-length" 31]
set maxPathLength [getParameter "max-path-length" 100]

foreach fileName [getSourceFileNames] {
    if {[string length $fileName] > $maxPathLength} {
        report $fileName 1 "path name too long"
    }

    set dirDepth 0
    foreach dir [file split [file dirname $fileName]] {
        if {$dir == "/" || $dir == "." || $dir == ".."} {
            continue
        }

        incr dirDepth

        if {[string length $dir] > $maxDirnameLength} {
            report $fileName 1 "directory name component too long"
            break
        }

        set first [string index $dir 0]
        if {[string is alpha $first] == 0 && $first != "_"} {
            report $fileName 1 "directory name should start with alphabetic character or underscore"
            break
        }

        if {[string first "." $dir] != -1} {
            report $fileName 1 "directory name should not contain the dot"
            break
        }
    }

    if {$dirDepth >= $maxDirectoryDepth} {
        report $fileName 1 "directory structure too deep"
    }

    set leafName [file tail $fileName]
    if {[string length $leafName] > $maxFilenameLength} {
        report $fileName 1 "file name too long"
    }

    set first [string index $leafName 0]
    if {[string is alpha $first] == 0 && $first != "_"} {
        report $fileName 1 "file name should start with alphabetic character or underscore"
    }

    if {[llength [split $leafName .]] > 2} {
        report $fileName 1 "file name should not contain more than one dot"
    }
}
