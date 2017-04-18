#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include <openthread/platform/alarm.h>

/**
 * diagnostics mode flag.
 *
 */
static bool sDiagMode = false;

void otPlatDiagProcess(int argc, char *argv[], char *aOutput, size_t aOutputMaxLen)
{
    // add more plarform specific diagnostics features here

    (void)argc;
}

void otPlatDiagModeSet(bool aMode)
{
    sDiagMode = aMode;
}

bool otPlatDiagModeGet(void)
{
    return sDiagMode;
}

