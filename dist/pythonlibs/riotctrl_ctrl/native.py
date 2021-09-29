# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import riotctrl.ctrl

import psutil


class NativeRIOTCtrl(riotctrl.ctrl.RIOTCtrl):
    """RIOTCtrl abstraction for a native node

    This works exactly as a normal RIOTCtrl, with the exception that
    `DEBUG_ADAPTER_ID` is set in the environment to the PID of the `native`
    process, whenever a terminal is started. This allows for `reset()` to also
    work for a the `native` instance.
    """

    def _set_debug_adapter_id(self, child):
        if child.name().endswith(".elf"):
            self.env["DEBUG_ADAPTER_ID"] = str(child.pid)
            return True
        return False

    def start_term(self, *args, **kwargs):
        super().start_term(*args, **kwargs)
        for child in psutil.Process(pid=self._term_pid()).children():
            if self._set_debug_adapter_id(child):
                break
