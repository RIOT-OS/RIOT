#!/bin/bash
# Script used generate sdkconfig_soc.h from ESP-IDF soc_caps.h
# by prefixing all defines with "CONFIG_*.
set -e

if [ $# -ne 3 ]; then
  echo "Usage: $0 <cpu_fam> <esp_sdk_dir> <bindir>"
  echo "Example: $0 esp32x build/pkg/esp32_sdk tests/sys/shell/bin/"
  exit 1
fi

CPU_FAM="$1"
ESP_SDK_DIR="$2"
BINDIR="$3"

SOC_CAPS_FILE="$ESP_SDK_DIR/components/soc/$CPU_FAM/include/soc/soc_caps.h"
OUTPUT_FILE="$BINDIR/sdkconfig_soc.h"

if [ ! -f "$SOC_CAPS_FILE" ]; then
  echo "Error: soc_caps.h not found at: $SOC_CAPS_FILE"
  exit 1
fi

if [ ! -d "$BINDIR" ]; then
  echo "Error: output directory not found: $BINDIR"
  exit 1
fi

sed -E '
  s/^#define[[:space:]]+([A-Z_0-9]+)/#define CONFIG_\1/
  s/\bSOC_([A-Z_0-9]+)/CONFIG_SOC_\1/g
' "$SOC_CAPS_FILE" >"$OUTPUT_FILE"

# In case the CPU is of type ESP32s3 the file mpu_caps.h that is included
# in soc_caps.h has to be parsed aswell.
if [ "$CPU_FAM" = "esp32s3" ]; then
  MPU_CAPS_FILE="$ESP_SDK_DIR/components/soc/$CPU_FAM/include/soc/mpu_caps.h"
  OUTPUT_MPU_FILE="$BINDIR/mpuconfig_caps.h"

  if [ ! -f "$MPU_CAPS_FILE" ]; then
    echo "Error: mpu_caps.h not found at: $MPU_CAPS_FILE"
    exit 1
  fi

  sed -E 's/^#define[[:space:]]+([A-Z_0-9]+)/#define CONFIG_\1/' \
    "$MPU_CAPS_FILE" >"$OUTPUT_MPU_FILE"

  sed -i -E 's/#include[[:space:]]+"mpu_caps\.h"/#include "mpuconfig_caps.h"/' \
    "$OUTPUT_FILE"
fi
