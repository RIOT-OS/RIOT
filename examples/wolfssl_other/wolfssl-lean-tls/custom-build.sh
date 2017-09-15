#!/bin/sh

PKG_DIR="../../../pkg/wolfssl"

echo "Preparing for custom build with local Makefile..."
echo "$PKG_DIR/Makefile.wolfssl $PKG_DIR/Makefile.original"
cp $PKG_DIR/Makefile.wolfssl $PKG_DIR/Makefile.original

echo "cp ./Makefile.wolfssl $PKG_DIR/Makefile.wolfssl"
cp ./Makefile.wolfssl $PKG_DIR/Makefile.wolfssl

echo "Building library..."
make

echo "Restoring original Makefile..."
echo "cp $PKG_DIR/Makefile.original $PKG_DIR/Makefile.wolfssl"
cp $PKG_DIR/Makefile.original $PKG_DIR/Makefile.wolfssl
