Test warning on conflicting features
==================================================
Using conflicting features provided by boards was invisible for the user until the used features resulted in a traceable problem or the user was aware of the conflict in advance from documentation etc.
Now, existing and known conflicts can be recorded into `FEATURES_CONFLICT` for each board to inform the user on a conflict situation during compile time.

This test requires conflicting features in its `Makefile`, i.e. `FEATURES_REQUIRED = periph_dac periph_spi`.
It is expected to be presented with a warning on the conflicts with a short description message during compile time for the [stm32f4discovery](https://github.com/RIOT-OS/RIOT/wiki/Board%3A-STM32F4discovery) by now, i.e. :

```
$ make BOARD=stm32f4discovery
The following features may conflict: periph_dac periph_spi
Rationale: On stm32f4discovery boards there are the same pins for the DAC and/or SPI_DEV(0).

EXPECT undesired behaviour!
```
The warning presents the conflicting features derived from `FEATURES_CONFLICT` and an optional message derived from `FEATURES_CONFLICT_MSG` provided int the `./RIOT/board/stm32f4discovery/Makefile.features`.

Whenever an application, such as this test, requires board features that match a _conflict group_, e.g. `FEATURES_REQUIRED = periph_dac periph_spi`, a similar warning to the above will be displayed during compile time.


---------
###Usage of _conflict groups_:

* Conflicting features are described in groups separated by a `:` (doublecolon) for each feature, e.g.:
`FEATURES_CONFLICT = periph_spi:periph_dac`, which states that `periph_spi` conflicts with `periph_dac`.
As seen above, this is the conflict of `SPI_DEV(0)` pinout is shared with `DAC` on the [stm32f4discovery](https://github.com/RIOT-OS/RIOT/wiki/Board%3A-STM32F4discovery) board.

* Distinct groups of conflicts are whitespace separated, e.g.:
`featureA:featureB featureC:featureD`, which states that `featureA` conflicts with `featureB`, and `featureC` conflicts with `featureD`.
This also means, that e.g. `FEATURES_REQUIRED = featureA featureD` would **not** produce a warning.

* The groups can have an arbitrary number of conflicting features, e.g.:
`featureA:featureB:featureC:featureX:featureY:featureZ`

* An optional information can be given using the `FEATURES_CONFLICT_MSG`, e.g.:
`FEATURES_CONFLICT_MSG = "featureA uses the same pins as featureB"`

* If the required features match multiple conflict groups, **ALL** conflicting features are provided to the user, e.g.:
`FEATURES_CONFLICT = featureA:featureB featureC:featureD` and
`FEATURES_REQUIRED = featureA featureB featureC featureD`
would result in: `The following features may conflict: featureA featureB featureC featureD`
