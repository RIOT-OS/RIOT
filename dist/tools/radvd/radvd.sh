#!/bin/sh

CURRENT_DIR="$(dirname $(readlink -f $0))"
PIDFILE="/tmp/radvd-${SUDO_USER}.pid"

stop_radvd() {
    if [ -f "${PIDFILE}" ]; then
        PID=$(cat ${PIDFILE})
    fi
    if [ -n "${PID}" ]; then
        kill ${PID}
        rm ${PIDFILE}
        echo "radvd stopped"
    fi
}

start_radvd() {
    export TAP
    export PREFIX
    cat ${CURRENT_DIR}/radvd.conf | envsubst | radvd -C /dev/stdin -u ${SUDO_USER} -p ${PIDFILE}

    if [ $? -ne 0 ]; then
        echo "radvd failed to start on ${TAP} with prefix ${PREFIX}"
        exit 1
    else
        echo "radvd running on ${TAP}"
    fi
}

usage() {
    echo "usage: $0 [options]" >&2
    echo "Options:" >&2
    echo "   -c <interface> <prefix>: Start radvd on <interface>, advertising <prefix>" >&2
    echo "   -d, --delete:             Stop radvd" >&2
}

if ! command -v radvd > /dev/null; then
    echo "Router Advertisement Daemon 'radvd' not found." >&2
    echo "Please install 'radvd' on your operating system." >&2
    exit 1
fi

while true ; do
    case "$1" in
        -c|--create)
            if [ -n "${COMMAND}" ]; then
                usage
                exit 2
            fi
            COMMAND="create"
            shift
            case "$1" in
                "")
                    usage
                    exit 2 ;;
                *)
                    TAP="$1"
                    shift 1 ;;
            esac
            case "$1" in
                "")
                    usage
                    exit 2 ;;
                *)
                    PREFIX="$1"
                    shift 1 ;;
            esac ;;
        -d|--delete)
            if [ -n "${COMMAND}" ]; then
                usage
                exit 2
            fi
            COMMAND="delete"
            shift ;;
        -h|--help)
            usage
            exit ;;
        "")
            break ;;
        *)  usage
            exit 2 ;;
    esac
done

if [ -z "${SUDO_USER}" ]; then
    echo 'Environment variable $SUDO_USER required; Please run with `sudo`'
    exit 1
fi

if [ -z "${COMMAND}" ]; then
    COMMAND="create"
fi

if [ "${COMMAND}" = 'create' ]; then
    [ -z "${TAP}" -o -z "${PREFIX}" ] && {
        usage
        exit 1
    }
    stop_radvd
    start_radvd || exit 1
elif [ "${COMMAND}" = 'delete' ]; then
    stop_radvd || exit 1
else
    echo 'unknown command'
    exit 1
fi
