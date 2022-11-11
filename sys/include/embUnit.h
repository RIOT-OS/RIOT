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
 * @brief RIOT Unittests based on the EmbUnit Framework
 *
 * @see https://sourceforge.net/projects/embunit
 *
 * @note Please refer to https://github.com/RIOT-OS/RIOT/wiki/Testing-RIOT
 * @note If mocking would be helpful for your unit test, you can also have a look at @ref pkg_fff.
 *
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef EMBUNIT_H
#define EMBUNIT_H

#include "embUnit/embUnit.h"

#ifdef OUTPUT
#   define OUTPUT_XML       (1)
#   define OUTPUT_TEXT      (2)
#   define OUTPUT_COMPILER  (4)
#   define OUTPUT_COLORTEXT (8)
#   define OUTPUT_COLOR     (16)

#   if (OUTPUT==OUTPUT_XML)
#       include "embUnit/XMLOutputter.h"
#       define OUTPUTTER   (XMLOutputter_outputter())
#   elif (OUTPUT==OUTPUT_TEXT)
#       include "embUnit/TextOutputter.h"
#       define OUTPUTTER   (TextOutputter_outputter())
#   elif (OUTPUT==OUTPUT_COMPILER)
#       include "embUnit/CompilerOutputter.h"
#       define OUTPUTTER   (CompilerOutputter_outputter())
#   elif (OUTPUT==OUTPUT_COLORTEXT)
#       include "embUnit/ColorTextOutputter.h"
#       define OUTPUTTER   (ColorTextOutputter_outputter())
#   elif (OUTPUT==OUTPUT_COLOR)
#       include "embUnit/ColorOutputter.h"
#       define OUTPUTTER   (ColorOutputter_outputter())
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

#endif /* EMBUNIT_H */
