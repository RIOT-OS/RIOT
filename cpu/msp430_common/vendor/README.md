The folder "msp430-gcc-support-files" has been imported from this URL:
https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/latest/exports/msp430-gcc-support-files-1.207.zip

Then cleaned up to remove currently unused (by RIOT) files:

    $ _CPUS="$(git grep -o '^CPU_MODEL.=.*430.*$' | cut -d' ' -f 3 | sort -u)"
    $ cd cpu/msp430_common/vendor/msp430-gcc-support-files/include
    $ rm $(ls | grep -v -E '(msp430\.h|in430\.h|legacy\.h|iomacros\.h)' | \
            grep -v -F "${_CPUS}" )
