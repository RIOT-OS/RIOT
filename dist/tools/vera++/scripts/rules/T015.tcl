#!/usr/bin/tclsh
# HTML links in comments and string literals should be correct

set urlRe {<[[:space:]]*[^>]*[[:space:]]+(?:HREF|SRC)[[:space:]]*=[[:space:]]*\"([^\"]*)\"}

foreach file [getSourceFileNames] {
    foreach token [getTokens $file 1 0 -1 -1 {ccomment cppcomment stringlit}] {
        set tokenValue [lindex $token 0]
        if {[regexp -nocase $urlRe $tokenValue dummy link] == 1} {

            if {[string index $link 0] == "\#" ||
                [string first "mailto:" $link] == 0 ||
                [string first "http:" $link] == 0 ||
                [string first "https:" $link] == 0 ||
                [string first "ftp:" $link] == 0 ||
                [string first "news:" $link] == 0 ||
                [string first "javascript:" $link] == 0} {
                continue
            }

            set lineNumber [lindex $token 1]

            if {[string first "file:" $link] == 0} {
                report $file $lineNumber "URL links to files are not allowed"
                continue
            }

            if {[regexp {[ \<\>\'\{\}\|\\\^\[\]]} $link] == 1} {
                report $file $lineNumber "URL link contains illegal character(s)"
                continue
            }

            set plainLink $link
            set pos [string first "\#" $link]
            if {$pos != -1} {
                set plainLink [string range $link 0 [expr $pos - 1]]
            }

            if {[string first "\#" $link [expr $pos + 1]] != -1} {
                report $file $lineNumber "URL link contains invalid bookmark"
            }

            set completeLink [file join [file dirname $file] $plainLink]
            if {[file isfile $completeLink] == 0} {
                report $file $lineNumber "URL points to non-existing file"
            }
        }
    }
}
