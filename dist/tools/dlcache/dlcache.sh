#!/usr/bin/env bash

DLCACHE_DIR=${DLCACHE_DIR:-~/.dlcache}

mkdir -p "$DLCACHE_DIR"

_echo() {
    echo "$@" 1>&2
}

if [ "$(uname)" = Darwin ]; then
    _locked() {
        local lockfile="$1"
        shift

        while ! shlock -p "$$" -f "$lockfile"; do
            sleep 0.2
        done

        "$@"

        rm "$lockfile"
    }
else
    _locked() {
        local lockfile="$1"
        shift

        (
        flock -w 600 9 || exit 1
        "$@"
        ) 9>"$lockfile"
    }
fi

# shasum is supported on Linux and Darwin
SHA512="shasum -a 512"

calcsha512() {
    local file="$1"
    local sha512="$2"
    local file_sha512
    file_sha512=$(${SHA512} "$file" | cut -d\  -f1)

    test "$sha512" = "$file_sha512"
}

downloader() {
    if [ -n "$(command -v wget)" ]; then
        wget -nv "$1" -O "$2"
    elif [ -n "$(command -v curl)" ]; then
        curl -L "$1" -o "$2"
    else
        _echo "$0: neither wget nor curl available!"
        return 1
    fi
}

download() {
    local url="$2"
    local _sha512="$3"
    local basename_url
    basename_url="$(basename "${url}")"
    local target="${1:-"${basename_url}"}"

    if [ -f "$target" ] && [ -n "$_sha512" ]; then
        # if our target file exists and an SHA512 sum was given, check it's SHA512
        calcsha512 "$target" "$_sha512" && {
            _echo "$0: target exists, SHA512 matches."
            exit 0
        }
    fi

    local filename
    filename="$(basename "$url")"
    [ -f "$DLCACHE_DIR/$filename" ] && {
        # if the file exists in cache, check it's SHA512 and possibly remove it.
        if calcsha512 "$DLCACHE_DIR/$filename" "$_sha512"; then
            _echo "$0: getting \"$url\" from cache"
        else
            _echo "$0: \"$DLCACHE_DIR/$filename\" has wrong or no checksum, re-downloading"
            rm "$DLCACHE_DIR/$filename"
        fi
    }

    [ ! -f "$DLCACHE_DIR/$filename" ] && {
        _echo "$0: downloading \"$url\"..."
        downloader "$url" "$DLCACHE_DIR/$filename" || {
            _echo "$0: error downloading $url to $DLCACHE_DIR/$filename!"
            exit 1
        }
        _echo "$0: done downloading \"$url\""
    }

    # only try to calculate the checksum if a checksum was given
    if [ -n "$_sha512" ]; then
        calcsha512 "$DLCACHE_DIR/$filename" "$_sha512" || {
            _echo "$0: checksum mismatch!"
            exit 1
        }
    fi

    if [ "$target" = "-" ]; then
        cat "$DLCACHE_DIR/$filename"
    else
        cp "$DLCACHE_DIR/$filename" "$target"
    fi
}

_locked "$DLCACHE_DIR/$(basename "$1").locked" download "$@"
