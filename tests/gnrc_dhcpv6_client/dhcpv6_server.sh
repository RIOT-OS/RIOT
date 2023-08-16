#! /bin/bash
#
# random_port.sh
# Copyright (C) 2018 Martine Lenders <m.lenders@fu-berlin.de>
#
# Distributed under terms of the MIT license.
#

if ! command -v kea-dhcp6 > /dev/null; then
  echo -e "\033[31;1mCommand kea-dhcp6 required\033[0m" >&2
  exit 1
fi

_sem_ver() {
    major=$(echo "$1" | cut -d. -f1 | sed -e 's/^0\+//' -e 's/^$/0/')
    minor=$(echo "$1" | cut -d. -f2 | sed -e 's/^0\+//' -e 's/^$/0/')
    patch=$(echo "$1" | cut -d. -f3 | sed -e 's/^0\+//' -e 's/^$/0/')
    echo "$(( major << 32 | minor << 8 | patch ))"
}

_kea_version_lesser_1_7_10() {
    test "$(_sem_ver "$(kea-dhcp6 -v)")" -lt "$(_sem_ver "1.7.10")"
    return $?
}

_dhcpv6_server() {
    TMPDIR="$(mktemp -u | xargs dirname)/${APPLICATION}_kea"
    CONFIG="${TMPDIR}/kea-dhcp6.config"

    # only used `mktemp` with dry-run above to get temp directory name, so we
    # still need to create the directory
    mkdir -p "${TMPDIR}"

    echo "Running kea-dhcpv6 in data directory ${TMPDIR}"
    if [ -f "${TMPDIR}/kea-dhcp6.kea-dhcp6.pid" ]; then
        # Kill Kea instance from potential previous run:w
        kill "$(cat "${TMPDIR}/kea-dhcp6.kea-dhcp6.pid")"
    fi
    sed "s/\"{{\s*env\.IFACE\s*}}\"/\"${IFACE}\"/" "$2" > "${CONFIG}"
    if ! _kea_version_lesser_1_7_10; then
        # Top-level "Logging" config is not supported by Kea >=1.7.10, so move
        # that config to Dhcp6 map, see
        # https://kea.readthedocs.io/en/kea-1.7.10/arm/config.html#json-syntax
        convert_json="$(cat <<EOF
import json

with open('$CONFIG') as c:
    config = json.load(c)
logging = config.pop('Logging', None)
if logging is not None:
    config['Dhcp6'].update(logging)
with open('$CONFIG', 'w') as c:
    json.dump(config, c)
EOF
)"
        python3 -c "$convert_json"
    fi
    sleep 1 # sleep to let TAP become active
    cd "$TMPDIR" || exit 1
    KEA_PIDFILE_DIR=. KEA_LOCKFILE_DIR=. \
        kea-dhcp6 -p "$1" -c "$CONFIG" &
}

_dhcpv6_server "$1" "$2" &
