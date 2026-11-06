<!--
SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
SPDX-License-Identifier: LGPL-2.1-only
-->
@defgroup pkg_radiolib RadioLib
@ingroup  pkg
@brief    Universal wireless communication library for embedded devices
@see      <https://github.com/jgromes/RadioLib>

@warning RadioLib is a C++ library and this package exposes a C++-only API:
`radiolib_radio` and `RiotHal` are only declared under `#ifdef __cplusplus`.
Applications using this package must be built as C++ as plain C applications
are not supported yet.

## Introduction

RadioLib provides a common API for a wide range of radio transceivers,
modems and radio shields. This package provides the RIOT hardware
abstraction layer (`RiotHal`) needed by RadioLib.

## Usage

Add the package, a transceiver driver module (e.g. `radiolib_sx127x`) and
the auto-init module to the Makefile of your application:

```makefile
USEPKG += radiolib
USEMODULE += radiolib_sx127x
USEMODULE += auto_init_radiolib
```

`auto_init_radiolib` brings up the transceiver selected by the enabled
driver module, using the pin/SPI configuration from
`radiolib_riotos_params.h` (override the `RADIOLIB_RIOTOS_PARAM_*` macros
from your board or application to match your wiring). The radio is then
available through the global `radiolib_radio` handle, or `NULL` if
auto-init failed:

```cpp
#include "radiolib_riotos.h"

if (radiolib_radio != NULL) {
    radiolib_radio->transmit("hello");
}
```

## Limitations

Only a subset of the transceiver families supported by RadioLib is compiled
in, see `RADIOLIB_MODULES` in `pkg/radiolib/Makefile`.

## License

RadioLib is licensed under the MIT license.
