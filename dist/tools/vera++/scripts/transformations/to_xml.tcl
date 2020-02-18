#!/usr/bin/tclsh
# transform the source file into XML file

foreach f [getSourceFileNames] {
    set outFileName "${f}.xml"
    set outFile [open $outFileName "w"]
    puts $outFile "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
    puts $outFile "<cpp-source file-name=\"${f}\">"
    foreach t [getTokens $f 1 0 -1 -1 {}] {
        set value [lindex $t 0]
        set line [lindex $t 1]
        set column [lindex $t 2]
        set name [lindex $t 3]

        if {$value == "\n"} {
            set value "!\[CDATA\[\n\]]"
        } else {
            set value [regsub -all "&" $value {\&amp;}]
            set value [regsub -all "<" $value {\&lt;}]
            set value [regsub -all ">" $value {\&gt;}]
        }
        puts $outFile "    <token name=\"${name}\" line=\"${line}\" column=\"${column}\">${value}</token>"
    }
    puts $outFile "</cpp-source>"
    close $outFile
}
