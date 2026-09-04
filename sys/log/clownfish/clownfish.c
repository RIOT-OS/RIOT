/*
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-FileCopyrightText: 2026 Carl Seifert <carl.seifert@tu-dresden.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>
#include "log_module.h"
#include "modules.h"

void _clownfish_print_prologue(
    const char* levelstr, const char* unit, const char* file, unsigned int line, 
    const char* func, const char* thread
) {
    if (IS_ACTIVE(CONFIG_LOG_SHOW_FILE) && file) {
        _fprint(LOG_STREAM, LOG_STYLE_FILE "%s:%u:\n" ANSI_STYLE_RESET, file, line);
    }
    _fprint(LOG_STREAM, levelstr);
    _fprint(LOG_STREAM, ANSI_STYLE_RESET " # ");
    if (IS_ACTIVE(CONFIG_LOG_SHOW_UNIT) && strlen(unit) > 0) {
        _fprint(LOG_STREAM, LOG_STYLE_UNIT);
        _fprint(LOG_STREAM, unit);
    }
    if (IS_ACTIVE(CONFIG_LOG_SHOW_FUNC) && IS_ACTIVE(CONFIG_LOG_SHOW_THREAD)) {
        _fprint(LOG_STREAM, LOG_STYLE_EXTRAS " (%s@%s): " ANSI_STYLE_RESET, func, thread);
    }
    else if (IS_ACTIVE(CONFIG_LOG_SHOW_FUNC)) {
        _fprint(LOG_STREAM, LOG_STYLE_EXTRAS " (%s): " ANSI_STYLE_RESET, func);
    }
    else if (IS_ACTIVE(CONFIG_LOG_SHOW_THREAD)) {
        _fprint(LOG_STREAM, LOG_STYLE_EXTRAS " (@%s): " ANSI_STYLE_RESET, thread);
    }
    else {
        _fprint(LOG_STREAM, LOG_STYLE_EXTRAS ": " ANSI_STYLE_RESET);
    }
}
