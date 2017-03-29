BEGIN {
    nb_elem = split(LIST, elem, " ")
}
($0 ~ VAR){
    l = length($1) + length(FS)
    new_line[0] = $1 FS
    l_align = length(new_line[0]) + 1
    i = 0
    for (j=0; j <= nb_elem; j++) {
        l_line = length(new_line[i]) + length(elem[j])
        if (l_line < 85) {
            new_line[i] = new_line[i] elem[j]
        } else {
            new_line[i] = new_line[i] "\\"
            i++
            for (k=0; k < l_align; k++)
                new_line[i] = new_line[i] " "
            new_line[i] = new_line[i] elem[j]
        }
        if (j < nb_elem)
            new_line[i] = new_line[i] " "
    }
    for (k=0; k<=i; k++) {
        print new_line[k]
    }
    next
}
{ print $0 }
