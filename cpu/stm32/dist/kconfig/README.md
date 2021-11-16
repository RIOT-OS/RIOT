## STM32 CPU lines and models Kconfig generator

The script `gen_kconfig.py` can be used to automatically generate the Kconfig
files describing CPU lines and models, per families (f0, f1, etc) and from
the ProductsList.xlsx sheet that are downloadable on the ST website.

### Prepare the data

The sheet are available from
https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html.
Just select a CPU series (e.g family in RIOT) in the menu on the left and go in
the product selector tab, then click the `Export` button to download the Excel
sheet (the default filename is `ProductsList.xlsx`).

The available CPU lines are extracted from the
`${RIOTBASE}/build/stm32/cmsis/<fam>/Include` directory. This means that the
headers of a given family are already fetched here. This can be done with the
following `make` commands:

```
$ cd $RIOTBASE
$ RIOTBASE=$(pwd) RIOTTOOLS=$(pwd)/dist/tools CPU_FAM=<cpu_fam> make -f cpu/stm32/Makefile.cmsis
```

`<cpu_fam>` can be any family in `f0`, `f1`, etc


### `gen_kconfig.py` dependencies

The script depends on `jinja2` templating engine to generate the Kconfig files
and `xlrd` to load and parse the Excel sheets. The dependencies can be
installed with `pip`:

```
$ pip install -r ./cpu/stm32/dist/kconfig/requirements.txt
```

### `gen_kconfig.py` usage

The script can be used to generate the `Kconfig.lines` and `Kconfig.models` of
a given family as follows:

```
$ cd $RIOTBASE
$ ./cpu/stm32/dist/kconfig/gen_kconfig.py ${RIOTBASE}/build/stm32/cmsis/<fam>/Include <cpu_fam> --sheets <path-to-sheet>/ProductsList.xlsx
```

The `--sheets` option can take several files. This allows to handle the L4 case
where the list of models is available in 2 separate sheets. So for L4 family,
the command should be:

```
$ cd $RIOTBASE
$ ./cpu/stm32/dist/kconfig/gen_kconfig.py ${RIOTBASE}/build/stm32/cmsis/l4/Include l4 --sheets <path-to-sheet>/L4ProductsList.xlsx <path-to-sheet>/L4+ProductsList.xlsx
```

By default, if the `Kconfig.lines` and `Kconfig.models` files of a given family
were not already created, they are created.
If the `Kconfig.lines` and `Kconfig.models` files of a given family are already
available in RIOT, by default the script will just dump the content to stdout.
The files can still be overwritten by using the `--overwrite` flag.

Print the detailed usage with `--help`:

```
$ ./cpu/stm32/dist/kconfig/gen_kconfig.py --help
usage: gen_kconfig.py [-h] [--sheets SHEETS [SHEETS ...]] [--overwrite] [--quiet] cmsis_dir cpu_fam

positional arguments:
  cmsis_dir             STM32 CMSIS directory
  cpu_fam               STM32 CPU Family

optional arguments:
  -h, --help            show this help message and exit
  --sheets SHEETS [SHEETS ...], -s SHEETS [SHEETS ...]
                        Excel sheet containing the list of products
  --overwrite, -o       Overwrite any existing Kconfig file
  --quiet, -q           Be quiet
```
