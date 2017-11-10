# Usage: awk -f <this_script> vendor/MKxxxx.h vendor/MKxxyy.h vendor/MKzzz.h ...
/Processor[s]?:/ {
    i=0;
    if (FNR == NR) {
        printf "#if";
    } else {
        printf "#elif";
    }
    while(match($0, /MK.*/)) {
        if (i>0) {
            printf " || \\\n   ";
        }
        printf " defined(CPU_MODEL_%s)", substr($0, RSTART, RLENGTH);
        getline;
        i++;
    }
    printf("\n#include \"%s\"\n", FILENAME);
    nextfile;
}
END {
  print "#endif"
}
