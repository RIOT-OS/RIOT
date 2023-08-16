#!/usr/bin/tclsh
# Reserved names should not be used for preprocessor macros

set keywords {
    asm
    auto
    bool
    break
    case
    catch
    char
    class
    const
    const_cast
    continue
    default
    delete
    goto
    do
    double
    dynamic_cast
    else
    enum
    explicit
    export
    extern
    false
    float
    for
    friend
    if
    inline
    int
    long
    mutable
    namespace
    new
    operator
    private
    protected
    public
    register
    reinterpret_cast
    return
    short
    signed
    sizeof
    static
    static_cast
    struct
    switch
    template
    this
    throw
    true
    try
    typedef
    typeid
    typename
    union
    unsigned
    using
    virtual
    void
    volatile
    wchar_t
    while

    and
    and_eq
    bitand
    bitor
    compl
    not
    not_eq
    or
    or_eq
    xor
    xor_eq
}

foreach f [getSourceFileNames] {
    foreach t [getTokens $f 1 0 -1 -1 {pp_define}] {
        set lineNumber [lindex $t 1]

        set line [getLine $f $lineNumber]
        set rest [string trimleft [string range $line \
                                       [expr [lindex $t 2] + [string length [lindex $t 0]]] end]]
        set macroName [string range $rest 0 [expr [string wordend $rest 0] - 1]]

        if {([regexp {^_} $macroName] && [string is upper -strict [string index $macroName 1]]) ||
            [string first "__" $macroName] != -1} {
            report $f $lineNumber "reserved name used for macro (incorrect use of underscore)"
        }
        if {[lsearch $keywords $macroName] != -1} {
            report $f $lineNumber "reserved name used for macro (keyword or alternative token redefined)"
        }
    }
}
