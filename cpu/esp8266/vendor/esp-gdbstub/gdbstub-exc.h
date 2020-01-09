#ifndef GDBSTUB_EXC_H
#define GDBSTUB_EXC_H

#include "xtensa/xtensa_context.h"

/**
 * @brief   Structure of additional registers in excpeption fram as used by GDB
 */
STRUCT_BEGIN
STRUCT_FIELD (long, 4, XT_STK_LITBASE, litbase)
STRUCT_FIELD (long, 4, XT_STK_SR176,   sr176)
STRUCT_FIELD (long, 4, XT_STK_SR208,   sr208)
/*
 * 'reason' is abused for both the debug and the exception vector:
 * if bit 7 is set, this contains an exception reason, otherwise it
 * contains a debug vector bitmap.
 */
STRUCT_FIELD (long, 4, XT_STK_REASON,  reason)
STRUCT_END(XtExcFrameGdb)

#ifdef __cplusplus
extern "C" {
#endif

#endif /* GDBSTUB_EXC_H */
