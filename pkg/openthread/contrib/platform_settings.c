/*
 * SPDX-FileCopyrightText: 2019 The OpenThread Authors
 * All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread radio platform abstraction
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <openthread/instance.h>
#include <openthread/platform/settings.h>

#define SETTINGS_BUFFER_SIZE 1024

static uint8_t sSettingsBuf[SETTINGS_BUFFER_SIZE];
static uint16_t sSettingsBufLength;

OT_TOOL_PACKED_BEGIN
struct settingsBlock {
    uint16_t key;
    uint16_t length;
} OT_TOOL_PACKED_END;

// settings API
void otPlatSettingsInit(otInstance *aInstance, const uint16_t *aSensitiveKeys,
                        uint16_t aSensitiveKeysLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aSensitiveKeys);
    OT_UNUSED_VARIABLE(aSensitiveKeysLength);

    sSettingsBufLength = 0;
}

void otPlatSettingsDeinit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue,
                          uint16_t *aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);

    uint16_t i = 0;
    uint16_t valueLength = 0;
    uint16_t readLength;
    int currentIndex = 0;
    const struct settingsBlock *currentBlock;
    otError error = OT_ERROR_NOT_FOUND;

    while (i < sSettingsBufLength) {
        currentBlock = (struct settingsBlock *)&sSettingsBuf[i];

        if (aKey == currentBlock->key) {
            if (currentIndex == aIndex) {
                readLength = currentBlock->length;

                // Perform read only if an input buffer was passed in
                if (aValue != NULL && aValueLength != NULL) {
                    // Adjust read length if input buffer size is smaller
                    if (readLength > *aValueLength) {
                        readLength = *aValueLength;
                    }

                    memcpy(aValue, &sSettingsBuf[i + sizeof(struct settingsBlock)], readLength);
                }

                valueLength = currentBlock->length;
                error = OT_ERROR_NONE;
                break;
            }

            currentIndex++;
        }

        i += sizeof(struct settingsBlock) + currentBlock->length;
    }

    if (aValueLength != NULL) {
        *aValueLength = valueLength;
    }

    return error;
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue,
                          uint16_t aValueLength)
{
    uint16_t i = 0;
    uint16_t currentBlockLength;
    uint16_t nextBlockStart;
    const struct settingsBlock *currentBlock;

    // Delete all entries of aKey
    while (i < sSettingsBufLength) {
        currentBlock = (struct settingsBlock *)&sSettingsBuf[i];
        currentBlockLength = sizeof(struct settingsBlock) + currentBlock->length;

        if (aKey == currentBlock->key) {
            nextBlockStart = i + currentBlockLength;

            if (nextBlockStart < sSettingsBufLength) {
                memmove(&sSettingsBuf[i], &sSettingsBuf[nextBlockStart],
                        sSettingsBufLength - nextBlockStart);
            }

            assert(sSettingsBufLength >= currentBlockLength);
            sSettingsBufLength -= currentBlockLength;
        }
        else {
            i += currentBlockLength;
        }
    }

    return otPlatSettingsAdd(aInstance, aKey, aValue, aValueLength);
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue,
                          uint16_t aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error;
    struct settingsBlock *currentBlock;
    const uint16_t newBlockLength = sizeof(struct settingsBlock) + aValueLength;

    if (sSettingsBufLength + newBlockLength <= sizeof(sSettingsBuf)) {
        currentBlock = (struct settingsBlock *)&sSettingsBuf[sSettingsBufLength];
        currentBlock->key = aKey;
        currentBlock->length = aValueLength;

        memcpy(&sSettingsBuf[sSettingsBufLength + sizeof(struct settingsBlock)], aValue,
               aValueLength);
        sSettingsBufLength += newBlockLength;

        error = OT_ERROR_NONE;
    }
    else {
        error = OT_ERROR_NO_BUFS;
    }

    return error;
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    OT_UNUSED_VARIABLE(aInstance);

    uint16_t i = 0;
    int currentIndex = 0;
    uint16_t nextBlockStart;
    uint16_t currentBlockLength;
    const struct settingsBlock *currentBlock;
    otError error = OT_ERROR_NOT_FOUND;

    while (i < sSettingsBufLength) {
        currentBlock = (struct settingsBlock *)&sSettingsBuf[i];
        currentBlockLength = sizeof(struct settingsBlock) + currentBlock->length;

        if (aKey == currentBlock->key) {
            if (currentIndex == aIndex) {
                nextBlockStart = i + currentBlockLength;

                if (nextBlockStart < sSettingsBufLength) {
                    memmove(&sSettingsBuf[i], &sSettingsBuf[nextBlockStart],
                            sSettingsBufLength - nextBlockStart);
                }

                assert(sSettingsBufLength >= currentBlockLength);
                sSettingsBufLength -= currentBlockLength;

                error = OT_ERROR_NONE;
                break;
            }
            else {
                currentIndex++;
            }
        }
        i += currentBlockLength;
    }

    return error;
}

void otPlatSettingsWipe(otInstance *aInstance)
{
    otPlatSettingsInit(aInstance, NULL, 0);
}
