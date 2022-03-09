#!/usr/bin/env python3
""" Menuconfig variant which uses RiotKconfig as base class """
import menuconfig
from riot_kconfig import standard_riot_kconfig

# keep documentation from the original tool
__doc__ = menuconfig.__doc__

if __name__ == "__main__":
    menuconfig.menuconfig(standard_riot_kconfig(__doc__))
