#!/bin/bash

set -e

gen_manifest() {
   local out="$1"
   shift
   local seqnr="$1"
   shift


   "${RIOTBASE}/dist/tools/suit/gen_manifest.py" \
     --urlroot "test://test" \
     --seqnr "$seqnr" \
     --uuid-vendor "riot-os.org" \
     --uuid-class "${BOARD}" \
     -o "$out.tmp" \
     "${@}"

    ${SUIT_TOOL} create -f suit -i "$out.tmp" -o "$out"

    rm -f "$out.tmp"
}

sign_manifest() {
  local in="$1"
  local out="$2"

  "${SUIT_TOOL}" sign -k "${SUIT_SEC}" -m "$in" -o "$out"
}

# random invalid data files
echo foo > "${MANIFEST_DIR}/file1.bin"
echo bar > "${MANIFEST_DIR}/file2.bin"

# random valid cbor (manifest but not signed, missing cose auth)
gen_manifest "${MANIFEST_DIR}/manifest0.bin" 1 "${MANIFEST_DIR}/file1.bin:$((0x1000))" "${MANIFEST_DIR}/file2.bin:$((0x2000))"

# manifest with invalid seqnr
sign_manifest "${MANIFEST_DIR}/manifest0.bin" "${MANIFEST_DIR}/manifest1.bin"

(BOARD=invalid gen_manifest "${MANIFEST_DIR}/manifest2.bin".unsigned 2 "${MANIFEST_DIR}/file1.bin:$((0x1000)):ram:0" "${MANIFEST_DIR}/file2.bin:$((0x2000)):ram:0")
sign_manifest "${MANIFEST_DIR}/manifest2.bin".unsigned "${MANIFEST_DIR}/manifest2.bin"

# valid manifest, valid seqnr, signed
gen_manifest "${MANIFEST_DIR}/manifest3.bin".unsigned 2 "${MANIFEST_DIR}/file1.bin:0:ram:0"
sign_manifest "${MANIFEST_DIR}/manifest3.bin".unsigned "${MANIFEST_DIR}/manifest3.bin"

# valid manifest, valid seqnr, signed, 2 components
gen_manifest "${MANIFEST_DIR}/manifest4.bin".unsigned 2 "${MANIFEST_DIR}/file1.bin:0:ram:0" "${MANIFEST_DIR}/file2.bin:0:ram:1"
sign_manifest "${MANIFEST_DIR}/manifest4.bin".unsigned "${MANIFEST_DIR}/manifest4.bin"
