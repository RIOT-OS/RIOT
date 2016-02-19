#!/bin/sh

# create the c interface and immplementation from .proto files
#

# @author Attilio Dona'

PROTOBUF_SRC=$1
echo dir: $PROTOBUF_SRC
protoc-c --c_out=$PROTOBUF_SRC --proto_path=/home/adona/ZAKKE/burba/cfg/protoc /home/adona/ZAKKE/burba/cfg/protoc/messages.proto