/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup unittests Unittests
 * @ingroup  sys
 * @{
 *
 * @file
 * @brief Common header file for unittests
 *
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __SYS__EMB_UNIT__H
#define __SYS__EMB_UNIT__H

#include "embUnit/embUnit.h"

#ifdef OUTPUT
#   define OUTPUT_XML      (1)
#   define OUTPUT_TEXT     (2)
#   define OUTPUT_COMPILER (4)

#   if (OUTPUT==OUTPUT_XML)
#       include "embUnit/XMLOutputter.h"
#       define OUTPUTTER   (XMLOutputter_outputter())
#   elif (OUTPUT==OUTPUT_TEXT)
#       include "embUnit/TextOutputter.h"
#       define OUTPUTTER   (TextOutputter_outputter())
#   elif (OUTPUT==OUTPUT_COMPILER)
#       include "embUnit/CompilerOutputter.h"
#       define OUTPUTTER   (CompilerOutputter_outputter())
#   endif

#   include "embUnit/TextUIRunner.h"

#   define TESTS_START()   TextUIRunner_start()
#   define TESTS_RUN(t)    TextUIRunner_runTest(t)
#   define TESTS_END()     TextUIRunner_end()
#else
#   define TESTS_START()   TestRunner_start()
#   define TESTS_RUN(t)    TestRunner_runTest(t)
#   define TESTS_END()     TestRunner_end()
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */
