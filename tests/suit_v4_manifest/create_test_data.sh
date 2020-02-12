#!/bin/bash

set -e

MANIFEST_DIR="${BINDIRBASE}/manifests"

gen_manifest() {
  local out="$1"
  shift
  local seqnr="$1"
  shift

  "${RIOTBASE}/dist/tools/suit_v4/gen_manifest.py" \
    --template "${RIOTBASE}/dist/tools/suit_v4/test-2img.json" \
    --urlroot "test://test" \
    --seqnr "$seqnr" \
    --uuid-vendor "riot-os.org" \
    --uuid-class "$BOARD" \
    --offsets 0x1000,0x2000 \
    -o "$out" \
    "$1" "$2"
}

sign_manifest() {
  local in="$1"
  local out="$2"

  "${RIOTBASE}/dist/tools/suit_v4/sign-04.py" "${SUIT_SEC}" "${SUIT_PUB}" "$in" "$out"
}

# random invalid manifest
echo foo > "${MANIFEST_DIR}/manifest0.bin"

# random valid cbor
gen_manifest "${MANIFEST_DIR}/manifest1.bin" 1 "${MANIFEST_DIR}/manifest0.bin" \
    "${MANIFEST_DIR}/manifest0.bin"

# manifest with invalid seqnr
sign_manifest "${MANIFEST_DIR}/manifest1.bin" "${MANIFEST_DIR}/manifest2.bin"

# manifest with valid signature and seqnr, invalid class id
( BOARD=invalid gen_manifest "${MANIFEST_DIR}/manifest3_unsigned.bin" 2 \
    "${MANIFEST_DIR}/manifest0.bin" "${MANIFEST_DIR}/manifest0.bin")
sign_manifest "${MANIFEST_DIR}/manifest3_unsigned.bin" "${MANIFEST_DIR}/manifest3.bin"

# manifest with valid signature and seqnr, valid class id, must pass the test
gen_manifest "${MANIFEST_DIR}/manifest4_unsigned.bin" 2 "${MANIFEST_DIR}/manifest0.bin" \
    "${MANIFEST_DIR}/manifest0.bin"
sign_manifest "${MANIFEST_DIR}/manifest4_unsigned.bin" "${MANIFEST_DIR}/manifest4.bin"
